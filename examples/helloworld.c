#include "revr/revr.h"
#include <stdio.h>

void hello(RevrRequest *req, RevrResponse *res) {
	revr_send_text(res, 200, "Hello, World!");
}

int main() {
	RevrApp *app = revr_app_create();

	revr_get(app, "/hello", hello);

	int status = revr_static(app, "/", "./examples/static_website");
	if (status != REVR_OK) {
		fprintf(stderr, "Error mounting directory! Error code: %d", status);
		return 1;
	}

	revr_listen(app, "3490");
	revr_app_free(app);
}
