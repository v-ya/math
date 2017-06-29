#include "dl.h"

static char *model_name="dl-v1.0.0";

_fun(model_name)
{
	ret->mode=type_string;
	ret->v.v_string=model_name;
}


int model_init(void)
{
	int err;
	err|=regfun("model_name",_func_model_name);
	return err;
}

void model_uini(void)
{

}

