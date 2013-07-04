/**
 *	BootThunder - Environment Variables.
 *
 *	@author James Walmsley
 *
 **/

#include <bitthunder.h>
#include <collections/bt_list.h>



typedef enum _ENV_TYPE {
	ENV_T_INTEGER,
	ENV_T_STRING,
} ENV_TYPE;

typedef struct _ENV_INT {
	BT_u32 i;
} ENV_INT;

typedef struct _ENV_VARIABLE {
	struct bt_list_head list;
	ENV_TYPE eType;
	union {
		ENV_INT	integer;
		ENV_INT string;
	} o;
} ENV_VARIABLE;
