char*
wsv_http_method_target_ressource(char* absolute_path,
								 char* abs_path_root,
								 char** abs_path_extension,
								 const char* location_root)
{
	char*			target_ressource;
	char*			it;
	char*			extension;

	++absolute_path;

	if (location_root != 0)
	{
		target_ressource = abs_path_root;
		it = abs_path_root;
		extension = 0;

		while ((*it = *location_root) != '\0')
		{
			++it;
			++location_root;
		}

		while ((*it = *absolute_path) != '\0')
		{
			if (*it == '.')
				extension = it;
			++it;
			++absolute_path;
		}

		if (extension == 0)
			*abs_path_extension = it;
		else
			*abs_path_extension = extension;

		return (target_ressource);
	}
	else
		return (absolute_path);
}
