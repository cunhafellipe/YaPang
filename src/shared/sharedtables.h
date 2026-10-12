// Original: d:\build\custom\temp\code\shared\sharedtables.h
#ifndef PG_SHARED_SHAREDTABLES_H
#define PG_SHARED_SHAREDTABLES_H

// Defined out of line here, not inline: the original emits it (public
// symbol, no caller) in client/ProjectG/s5/SupplyPack, the only client unit
// known to include this header. A second includer needs it made inline.

// sharedtables.h:904
bool IsLicenseLimitedItem(unsigned long typeId) {
	unsigned long limitedItems[] = {
		0x14000004,
	};

	for (int i = 0; i < (int)(sizeof(limitedItems) / sizeof(limitedItems[0])); i++) {

		if (limitedItems[i] == typeId)
			return true;
	}

	return false;
}

#endif // PG_SHARED_SHAREDTABLES_H
