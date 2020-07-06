
#include <cstring>
#include <algorithm>
#include <mysql/udf_registration_types.h>

#pragma pack(push, 1)
struct mysql_wkb {
	unsigned int srid;
	unsigned char endian_flag;
	unsigned int geometry_type;
};

struct extended_wkb {
	unsigned char endian_flag;
	unsigned int geometry_type;
	unsigned int srid;
};
#pragma pack(pop)

extern "C" {

char *st_asewkb(UDF_INIT *initid, UDF_ARGS *args, char *result, 
				unsigned long *length, char *is_null, char *error) {
	
	mysql_wkb data;
	extended_wkb target;
	
	std::copy_n(args->args[0], sizeof(mysql_wkb), (char *)&data);
	
	// 0x00 > big endian
	// 0x01 > little endian
	if (data.endian_flag > 1) {
		*is_null = 1;
		return nullptr;
	}
	
	target.endian_flag = data.endian_flag;
	target.srid = data.srid;
	target.geometry_type = data.geometry_type | (data.endian_flag == 1 ? 0x20000000 : 0x00000002);

	std::copy_n((char *)&target, sizeof(extended_wkb), result);
	std::copy_n(args->args[0] + sizeof(extended_wkb), 
		args->lengths[0] - sizeof(extended_wkb), 
		result + sizeof(extended_wkb));

	*length=args->lengths[0];

	return result;
}

bool st_asewkb_init (UDF_INIT *initid, UDF_ARGS *args, char *message)
{
	
	if (args->arg_count != 1 || args->arg_type[0] != STRING_RESULT) {
		strcpy(message, "requires exactly one binary argument");
		return 1;
	}

	if (args->lengths[0] < sizeof(mysql_wkb)) {
		strcpy(message, "invalid data length");
		return 1;
	}

	initid->max_length = args->lengths[0];

	return 0;
}
	
}
