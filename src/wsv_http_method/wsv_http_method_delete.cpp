#include "wsv_wrapper.h"
#include "wsv_retval.h"
#include "wsv_http_message.h"
#include "wsv_http_method.h"
#include "wsv_location.h"

int
wsv_http_method_delete(struct wsv_http_message_s* m, const void* location)
{
	char*							target_ressource;
	struct stat						statbuf;
	const struct wsv_location_s*	lc;

	lc = (const struct wsv_location_s*)location;

	target_ressource = wsv_http_method_target_ressource(m->absolute_path,
														m->abs_path_root,
														&m->abs_path_extension,
														lc->root);

	if (wsv_stat(target_ressource, &statbuf) == -1)
		return (WSV_ERROR_404);

	if (S_ISDIR(statbuf.st_mode))
		return (WSV_ERROR_404);

	if (wsv_unlink(target_ressource) == -1)
		return (WSV_ERROR_500);

	return (WSV_SUCCESS_204);
}
