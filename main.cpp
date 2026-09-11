#include "stdafx.h"

DECLARE_COMPONENT_VERSION(
// component name
COMPONENT_NAME_HC,
// component version
FOO_COMPONENT_VERSION,
"A foobar 2000 plugin featuring playback bookmarks\n\n"
"Author: da yuyu\n"
"Version: "FOO_COMPONENT_VERSION"\n"
"Compiled: "__DATE__ "\n"
"fb2k SDK: "PLUGIN_FB2K_SDK"\n\n"
"Website: https://github.com/ghDaYuYu/foo_vbookmark\n"
"Wiki: https://wiki.hydrogenaud.io/index.php?title=Foobar2000:Components/Vital_Bookmarks_(foo_vbookmark)\n"
"Discussion: https://hydrogenaud.io/index.php/topic,124481.0.html\n\n"
"Acknowledgements:\n\n"
"Thanks to Paremo for starting the foo_bbookmark project.\n"
"Original Author Website: https://github.com/Paremo/foo_bbookmark\n"
"Original Discussion: https://hydrogenaud.io/index.php/topic,118283.0.html\n\n"
"Disclaimer:\n"
"The software comes with no explicit or implicit promises regarding how well it will work.\n"
"This software is not error - free, secure, or will work without interruption.\n"
"Users should not be suing for financial loss, data loss, or broken business operations caused by bugs or crashes.\n"
"There is no promise that the software will suit a specific or unique need.\n"
"The creator is not responsible for any illegal acts caused by improper use.\n"
);

// prevent renaming
VALIDATE_COMPONENT_FILENAME(COMPONENT_NAME_DLL);
