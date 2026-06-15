#include "revr/revr.h"
#include <stdio.h>

void hello(RevrRequest *req, RevrResponse *res) {
	revr_send_text(res, 200, "Hello, World!");
}

void hi(RevrRequest *req, RevrResponse *res) {
	revr_send_html(res, 200, "<h1>Hi there!</h1>");
}

void report(RevrRequest *req, RevrResponse *res) {
	revr_send_file(res, 200, "./examples/assets/file.pdf");
}

void headers(RevrRequest *req, RevrResponse *res) {
	const char *user_agent = revr_req_header(req, "User-Agent");
	const char *host = revr_req_header(req, "Host");
	printf("Host: %s\n", host ? host : "(null)");
	printf("User-Agent: %s\n", user_agent ? user_agent : "(null)");

	revr_res_header(res, "X-Powered-By", "Revr");
	revr_res_header(res, "X-Test", "123");
	/* Test replacement logic */
	revr_res_header(res, "X-Test", "456");

	revr_send_text(res, 200, "Check terminal output and response headers.");
}

int main() {
	RevrApp *app = revr_app_create();

	revr_get(app, "/hello", hello);
	revr_get(app, "/hi", hi);
	revr_get(app, "/report", report);
	revr_get(app, "/headers", headers);

	int status = revr_static(app, "/", "./examples/static_website");
	if (status != REVR_OK) {
		fprintf(stderr, "Error mounting directory! Error code: %d", status);
		return 1;
	}

	revr_listen(app, "3490");
	revr_app_free(app);
}
