#include <stdlib.h>
#include <iostream>
#include <string>
#include <sstream>
#include <fcgio.h>
#include <libpq-fe.h>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

using namespace std;

using boost::property_tree::ptree;
using boost::property_tree::read_json;
using boost::property_tree::write_json;

// Maximum bytes
const unsigned long STDIN_MAX = 1000000;

/**
 * Note this is not thread safe due to the static allocation of the
 * content_buffer.
 */
string get_request_content(const FCGX_Request & request) {
    char * content_length_str = FCGX_GetParam("CONTENT_LENGTH", request.envp);
    unsigned long content_length = STDIN_MAX;

    if (content_length_str) {
        content_length = strtol(content_length_str, &content_length_str, 10);
        if (*content_length_str) {
          cerr << "Can't Parse 'CONTENT_LENGTH='"
                 << FCGX_GetParam("CONTENT_LENGTH", request.envp)
                 << "'. Consuming stdin up to " << STDIN_MAX << '\n';
        }

        if (content_length > STDIN_MAX) {
            content_length = STDIN_MAX;
        }
    } else {
        // Do not read from stdin if CONTENT_LENGTH is missing
        content_length = 0;
    }

    char * content_buffer = new char[content_length];
    cin.read(content_buffer, content_length);
    content_length = cin.gcount();

    // Chew up any remaining stdin - this shouldn't be necessary
    // but is because mod_fastcgi doesn't handle it correctly.

    // ignore() doesn't set the eof bit in some versions of glibc++
    // so use gcount() instead of eof()...
    do cin.ignore(1024); while (cin.gcount() == 1024);

    string content(content_buffer, content_length);
    delete [] content_buffer;
    return content;
}

static string get_response(const string & uri, const string & method, const string & contents) {
  ptree pt;

  PGconn *conn = PQconnectdb("user=sd_ventures dbname=sd_ventures_development hostaddr=127.0.0.1 port=5432");
  if (PQstatus(conn) != CONNECTION_OK) {
    pt.put("error", "failed to get connection");
  } else {
    PGresult *res = PQexec(conn, "SELECT * FROM devices");
    if (PQresultStatus(res) != PGRES_TUPLES_OK) {
      pt.put("error", "failed to get devices");
    } else {
      int nFields = PQnfields(res);
      int nRows = PQntuples(res);
      for (int i = 0; i < nRows; i++) {
        ptree row;
        for (int j = 0; j < nFields; j++) {
          row.put(PQfname(res, j), PQgetvalue(res, i, j));
        }
        pt.push_back(make_pair("", row));
      }
    }
    PQclear(res);
  }

  PQfinish(conn);
  ostringstream ss;
  write_json(ss, pt);
  return ss.str();
}


int main(void) {
    // Backup the stdio streambufs
  streambuf * cin_streambuf  = cin.rdbuf();
  streambuf * cout_streambuf = cout.rdbuf();
  streambuf * cerr_streambuf = cerr.rdbuf();

  FCGX_Request request;

  FCGX_Init();
  FCGX_InitRequest(&request, 0, 0);

  while (FCGX_Accept_r(&request) == 0) {
      fcgi_streambuf cin_fcgi_streambuf(request.in);
      fcgi_streambuf cout_fcgi_streambuf(request.out);
      fcgi_streambuf cerr_fcgi_streambuf(request.err);

      cin.rdbuf(&cin_fcgi_streambuf);
      cout.rdbuf(&cout_fcgi_streambuf);
      cerr.rdbuf(&cerr_fcgi_streambuf);

      string uri = FCGX_GetParam("REQUEST_URI", request.envp);
      string method = FCGX_GetParam("REQUEST_METHOD", request.envp);
      string contents = get_request_content(request);

      string response = get_response(uri, method, contents);

      cout << "Content-type: application/json\r\n" << response << '\n';
      // Note: the fcgi_streambuf destructor will auto flush
  }

  // restore stdio streambufs
  cin.rdbuf(cin_streambuf);
  cout.rdbuf(cout_streambuf);
  cerr.rdbuf(cerr_streambuf);

  return 0;
}
