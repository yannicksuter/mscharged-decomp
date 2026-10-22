#include <revolution/dvd.h>
#include <revolution/os.h>
#include <revolution/sc.h>

static funcptr_t FatalFunc = NULL;

const char* const __DVDErrorMessageDefault[] = {
    // SC_LANG_JP (Shift-JIS)
    "\n\n\n\x83\x47\x83\x89\x81\x5B\x82\xAA\x94\xAD\x90\xB6\x82\xB5\x82\xDC\x82\xB5\x82"
    "\xBD\x81\x42\n"
    "\n\x83\x43\x83\x57\x83\x46\x83\x4E\x83\x67\x83\x7B\x83\x5E\x83\x93\x82\xF0\x89\x9F"
    "\x82\xB5\x82\xC4\x83\x66\x83\x42\x83\x58\x83\x4E\x82\xF0\x8E\xE6\x82\xE8\x8F\x6F\x82"
    "\xB5\x82\xC4\x82\xA9\n"
    "\x82\xE7\x81\x41\x96\x7B\x91\xCC\x82\xCC\x93\x64\x8C\xB9\x82\xF0\x4F\x46\x46\x82\xC9"
    "\x82\xB5\x82\xC4\x81\x41\x96\x7B\x91\xCC\x82\xCC\x8E\xE6\x88\xB5\x90\xE0\x96\xBE\x8F"
    "\x91\x82\xCC\n"
    "\x8E\x77\x8E\xA6\x82\xC9\x8F\x5D\x82\xC1\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81"
    "\x42",

    // SC_LANG_EN
    "\n\n\nAn error has occurred.\n"
    "Press the Eject Button, remove the\n"
    "Game Disc, and turn off the power to the\n"
    "console. Please read the Wii Operations\n"
    "Manual for further instructions.",

    // SC_LANG_DE
    "\n\n\nEin Fehler ist aufgetreten.\n"
    "Dr\xFC""cke den Ausgabeknopf, entnimm die\n"
    "Disc und schalte die Wii-Konsole aus.\n"
    "Bitte lies die Wii-Bedienungsanleitung,\n"
    "um weitere Informationen zu erhalten.",

    // SC_LANG_FR
    "\n\n\nUne erreur est survenue.\n"
    "Appuyez sur le bouton EJECT, retirez\n"
    "le disque et \xE9teignez la console.\n"
    "Veuillez vous r\xE9""f\xE9rer au Mode d'emploi\n"
    "de la Wii pour plus de d\xE9tails.",

    // SC_LANG_SP
    "\n\n\nOcurri\xF3 un Error.\n"
    "Oprime el Bot\xF3n EJECT, saca el disco\n"
    "y apaga la consola. Consulta el manual\n"
    "de operaciones de la consola Wii para\n"
    "obtener m\xE1s informaci\xF3n.",

    // SC_LANG_IT
    "\n\n\nSi \xE8 verificato un errore.\n"
    "Premi il pulsante EJECT, estrai il disco\n"
    "e spegni la console. Per maggiori\n"
    "informazioni, consulta il manuale di\n"
    "istruzioni della console Wii.",

    // SC_LANG_NL
    "\n\n\nEr is een fout opgetreden.\n"
    "Druk op de EJECT-knop, verwijder de\n"
    "disk en zet het Wii-systeem uit. Lees\n"
    "de Wii-handleiding voor meer informatie."

};

const char* const __DVDErrorMessageEurope[] = {
    // SC_LANG_JP (Shift-JIS)
    "\n\n\n\x83\x47\x83\x89\x81\x5B\x82\xAA\x94\xAD\x90\xB6\x82\xB5\x82\xDC\x82\xB5\x82"
    "\xBD\x81\x42\n"
    "\n\x83\x43\x83\x57\x83\x46\x83\x4E\x83\x67\x83\x7B\x83\x5E\x83\x93\x82\xF0\x89\x9F"
    "\x82\xB5\x82\xC4\x83\x66\x83\x42\x83\x58\x83\x4E\x82\xF0\x8E\xE6\x82\xE8\x8F\x6F\x82"
    "\xB5\x82\xC4\x82\xA9\n"
    "\x82\xE7\x81\x41\x96\x7B\x91\xCC\x82\xCC\x93\x64\x8C\xB9\x82\xF0\x4F\x46\x46\x82\xC9"
    "\x82\xB5\x82\xC4\x81\x41\x96\x7B\x91\xCC\x82\xCC\x8E\xE6\x88\xB5\x90\xE0\x96\xBE\x8F"
    "\x91\x82\xCC\n"
    "\x8E\x77\x8E\xA6\x82\xC9\x8F\x5D\x82\xC1\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81"
    "\x42",

    // SC_LANG_EN
    "\n\n\nAn error has occurred.\n"
    "Press the EJECT Button, remove the Disc,\n"
    "and turn off the power to the console.\n"
    "Please refer to the Wii Operations Manual\n"
    "for details.",

    // SC_LANG_DE
    "\n\n\nEin Fehler ist aufgetreten.\n"
    "Dr\xFC""cke den Ausgabeknopf, entnimm die\n"
    "Disc und schalte die Wii-Konsole aus.\n"
    "Bitte lies die Wii-Bedienungsanleitung,\n"
    "um weitere Informationen zu erhalten.",

    // SC_LANG_FR
    "\n\n\nUne erreur est survenue.\n"
    "Appuyez sur le bouton EJECT, retirez\n"
    "le disque et \xE9teignez la console.\n"
    "Veuillez vous r\xE9""f\xE9rer au mode d'emploi\n"
    "Wii pour plus de d\xE9tails.",

    // SC_LANG_SP
    "\n\n\nSe ha producido un error.\n"
    "Pulsa el Bot\xF3n EJECT, extrae el disco y\n"
    "apaga la consola. Consulta el manual de\n"
    "instrucciones de la consola Wii para\n"
    "obtener m\xE1s informaci\xF3n.",

    // SC_LANG_IT
    "\n\n\nSi \xE8 verificato un errore.\n"
    "Premi il pulsante EJECT, estrai il disco\n"
    "e spegni la console. Per maggiori\n"
    "informazioni, consulta il manuale di\n"
    "istruzioni della console Wii.",

    // SC_LANG_NL
    "\n\n\nEr is een fout opgetreden.\n"
    "Druk op de EJECT-knop, verwijder de\n"
    "disk en zet het Wii-systeem uit. Lees\n"
    "de Wii-handleiding voor meer informatie."

};

const char* __DVDErrorMessage104[] = {
    // SC_LANG_JP (Shift-JIS)
    "\n\n\x83\x47\x83\x89\x81\x5B\x83\x52\x81\x5B\x83\x68\x82\x50\x82\x4F\x82\x53\x81\x42"
    "\n\x83\x47\x83\x89\x81\x5B\x82\xAA\x94\xAD\x90\xB6\x82\xB5\x82\xDC\x82\xB5\x82\xBD"
    "\x81\x42\n"
    "\n\x83\x43\x83\x57\x83\x46\x83\x4E\x83\x67\x83\x7B\x83\x5E\x83\x93\x82\xF0\x89\x9F"
    "\x82\xB5\x82\xC4\x83\x66\x83\x42\x83\x58\x83\x4E\x82\xF0\x8E\xE6\x82\xE8\x8F\x6F\x82"
    "\xB5\x82\xC4\x82\xA9\n"
    "\x82\xE7\x81\x41\x96\x7B\x91\xCC\x82\xCC\x93\x64\x8C\xB9\x82\xF0\x4F\x46\x46\x82\xC9"
    "\x82\xB5\x82\xC4\x81\x41\x96\x7B\x91\xCC\x82\xCC\x8E\xE6\x88\xB5\x90\xE0\x96\xBE\x8F"
    "\x91\x82\xCC\n"
    "\x8E\x77\x8E\xA6\x82\xC9\x8F\x5D\x82\xC1\x82\xC4\x82\xAD\x82\xBE\x82\xB3\x82\xA2\x81"
    "\x42",

    // SC_LANG_EN
    "\n\nError #104,\n"
    "An error has occurred.\n"
    "Press the EJECT Button, remove the\n"
    "Game Disc, and turn the power off.\n"
    "Please read the Wii Operations Manual\n"
    "for more information."

};

void __DVDShowFatalMessage(void) {
    const char* msg;
    const char* const* messages;
    GXColor bgColor = {0, 0, 0, 0};
    GXColor textColor = {255, 255, 255, 0};

    if (SCGetLanguage() == SC_LANG_JP) {
        OSSetFontEncode(OS_FONT_ENCODE_SJIS);
    } else {
        OSSetFontEncode(OS_FONT_ENCODE_ANSI);
    }

    switch (SCGetProductGameRegion()) {
    default:
        messages = __DVDErrorMessageDefault;
        break;
    case SC_PRD_GAME_REG_EU:
        messages = __DVDErrorMessageEurope;
        break;
    case SC_PRD_GAME_REG_KR:
    case SC_PRD_GAME_REG_CN:
        messages = __DVDErrorMessage104;
        break;
    }

    msg = (SCGetLanguage() > SC_LANG_NL) ? messages[SC_LANG_EN]
                                       : messages[SCGetLanguage()];
    OSFatal(textColor, bgColor, msg);
}

BOOL DVDSetAutoFatalMessaging(BOOL enable) {
    BOOL enabled;
    BOOL old;

    enabled = OSDisableInterrupts();

    old = (FatalFunc != NULL);

    if (enable) {
        FatalFunc = __DVDShowFatalMessage;
    } else {
        FatalFunc = NULL;
    }

    OSRestoreInterrupts(enabled);
    return old;
}

BOOL __DVDGetAutoFatalMessaging(void) {
    return FatalFunc != NULL;
}

void __DVDPrintFatalMessage(void) {
    if (FatalFunc != NULL) {
        FatalFunc();
    }
}
