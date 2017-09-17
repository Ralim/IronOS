/*
 * Strings.c
 *
 *  Created on: 5Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Strings.h"
#define LANG_IT
#ifdef LANG_EN
  const char* SettingsLongNames[14] = {
    /*These are all the help text for all the settings.*/
    /*All must start with 6 spaces so they come on screen nicely.*/
    "      Power source. Sets cutoff voltage. <DC=10V S=3.3V per cell>",
    "      Sleep Temperature <C>",
    "      Sleep Timeout <Minutes>",
    "      Shutdown Timeout <Minutes>",
    "      Motion Sensitivity <0=Off 1=Least sensitive 9=Most sensitive>",
    "      Temperature Unit <C=Celsius F=Farnheit>",
    "      Temperature Rounding Amount <Units>",
    "      Temperature Display Update Rate <S=Slow M=Medium F=Fast>",
    "      Display Orientation <A=Automatic L=Left Handed R=Right Handed>",
    "      Enable front key enters boost mode 450C mode when soldering <T=True F=False>",
    "      Temperature when in \"boost\" mode <C>",
    "      Changes the arrows to a power display when soldering <T=True F=False>",
    "      Automatically starts the iron into soldering on power up. <T=Soldering S=Sleep mode F=Off>",
    "      Blink the temperature on the cooling screen while the tip is still hot. <T=True F=False>"
  };
  const char* TempCalStatus[3] = { "Cal Temp", "Cal OK  ", "Cal Fail" }; //All fixed 8 chars
  const char* UVLOWarningString = "Low Volt"; //Fixed width 8 chars
  const char* CoolingPromptString = "Off  "; //Fixed width 5 chars
  const char SettingTrueChar = 'T';
  const char SettingFalseChar = 'F';
  const char SettingSleepChar = 'S';
  const char SettingFastChar = 'F';
  const char SettingMediumChar = 'M';
  const char SettingSlowChar = 'S';
  const char SettingRightChar = 'R';
  const char SettingLeftChar = 'L';
  const char SettingAutoChar = 'A';
  const char SettingTempCChar = 'C';
  const char SettingTempFChar = 'F';
#endif

#ifdef LANG_ES
  const char* SettingsLongNames[14] = {
    /*These are all the help text for all the settings.*/
    /*All must start with 6 spaces so they come on screen nicely.*/
    "      Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
    "      Temperatura en reposo. <C>",
    "      Tiempo hasta activar reposo. <Minutos>",
    "      Tiempo hasta apagado. <Minutos>",
    "      Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
    "      Unidad de temperatura.",
    "      Redondeo de la temperatura.",
    "      Tasa de actualización de la temperatura.",
    "      Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
    "      Activar el botón \"Boost\" en modo soldadura.",
    "      Temperatura en modo \"Boost\". <C>",
    "      Cambiar las flechas en pantalla por indicador de potencia en modo soldadura.",
    "      Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
    "      Parpadea la temperatura en el enfriamiento si la punta sigue caliente."
  };
  const char* TempCalStatus[3] = {"CAL TEMP", "CAL OK  ", "CAL FAIL"}; //All fixed 8 chars
  const char* UVLOWarningString = "LOW VOLT"; //Fixed width 8 chars
  const char* CoolingPromptString = "Cool "; //Fixed width 5 chars
  const char SettingTrueChar = 'V';
  const char SettingFalseChar = 'F';
  const char SettingSleepChar = 'S';
  const char SettingFastChar = 'R';
  const char SettingMediumChar = 'M';
  const char SettingSlowChar = 'L';
  const char SettingRightChar = 'D';
  const char SettingLeftChar = 'I';
  const char SettingAutoChar = 'A';
  const char SettingTempCChar = 'C';
  const char SettingTempFChar = 'F';
#endif

#ifdef LANG_DE
  const char* SettingsLongNames[14] = {
    /*These are all the help text for all the settings.*/
    /*All must start with 6 spaces so they come on screen nicely.*/
    "      Stromversorgung. Setzt Abschaltspannung <DC=10V S=3.3V pro Zelle>",
    "      Ruhetemperatur <C>",
    "      Ruhemodus nach <Minuten>",
    "      Abschaltzeit <Minuten>",
    "      Bewegungsempfindlichkeit <0=Aus 1=Minimal 9=Maximal>",
    "      Temperatur Einheit",
    "      Temperatur Runden",
    "      Temperaturanzeige Updaterate",
    "      Anzeigerichtung <A=Auto L=Linkshändig R=Rechtshändig>",
    "      Fronttaste für Temperaturboost einschalten",
    "      Temperatur im \"boost\"-Modus <C>",
    "      Ändert \"Temperaturpfeile\" in Leistungsbalken",
    "      Automatischer Start beim Einschalten. <J=Löttemp R=Ruhemodus N=Aus>",
    "      Temperatur blinkt beim Abkühlen, solange noch heiß."
  };
  const char* TempCalStatus[3] = { "Cal Temp", "Cal OK  ", "Cal Fehl" }; //All fixed 8 chars
  const char* UVLOWarningString = "V gering"; //Fixed width 8 chars
  const char* CoolingPromptString = "Kalt "; //Fixed width 5 chars
  const char SettingTrueChar = 'J';
  const char SettingFalseChar = 'N';
  const char SettingSleepChar = 'R';
  const char SettingFastChar = 'S';
  const char SettingMediumChar = 'M';
  const char SettingSlowChar = 'L';
  const char SettingRightChar = 'R';
  const char SettingLeftChar = 'L';
  const char SettingAutoChar = 'A';
  const char SettingTempCChar = 'C';
  const char SettingTempFChar = 'F';
#endif

#ifdef LANG_FR
  const char* SettingsLongNames[14] = {
    /*These are all the help text for all the settings.*/
    /*All must start with 6 spaces so they come on screen nicely.*/
    "      Type d\'alimentation. Regle la tension de coupure. <DC=10V S=3.3V par cellules>",
    "      Temperature en veille. <C>",
    "      Temps avant mise en veille. <Minutes>",
    "      Temps avant extinction. <Minutes>",
    "      Sensibilitee du capteur de mouvement. <0=Inactif 1=Peu sensible 9=Tres sensible>",
    "      Unitee de temperature.",
    "      Arrondissement de la temperature.",
    "      Taux de raffraichissement de la temperature.",
    "      Orientation de l\'affichage. <A=Automatique G=Gaucher D=Droitier>",
    "      Active le mode \"Boost\" 450C sur le bouton de devant pendant la soudure.",
    "      Temperature du mode \"Boost\". <C>",
    "      Change les fleches en affichage de la tension pendant la soudure.",
    "      Demarre automatiquement la soudure a l\'allumage. <A=Active, V=Mode Veille, D=Desactive>",
    "      Fait clignotter la temperature pendant la phase de refroidissement quand la panne est chaude."
  };
  const char* TempCalStatus[3] = { "Cal Temp", "Cal OK  ", "Cal Err " }; //All fixed 8 chars
  const char* UVLOWarningString = "Batt Bas"; //Fixed width 8 chars
  const char* CoolingPromptString = "Etein"; //Fixed width 5 chars
  const char SettingTrueChar = 'A';
  const char SettingFalseChar = 'D';
  const char SettingSleepChar = 'V';
  const char SettingFastChar = 'R';
  const char SettingMediumChar = 'M';
  const char SettingSlowChar = 'L';
  const char SettingRightChar = 'D';
  const char SettingLeftChar = 'G';
  const char SettingAutoChar = 'A';
  const char SettingTempCChar = 'C';
  const char SettingTempFChar = 'F';
#endif

#ifdef LANG_IT
  const char* SettingsLongNames[14] = {
    /*These are all the help text for all the settings.*/
    /*All must start with 6 spaces so they come on screen nicely.*/
    "      Sorgente di alimentazione. Imposta il limite inferiore di tensione. <DC=10V S=3.3V per cella>",
    "      Temperatura modalità riposo <C>",
    "      Timeout per passaggio a modalità riposo <Minuti>",
    "      Timeout spegnimento <Minuti>",
    "      Sensibilità al movimento <0=Spento 1=Sensibilità minima 9=Sensibilità massima>",
    "      Unità di temperatura",
    "      Arrotondamento temperatura",
    "      Velocità di aggiornamento temperatura",
    "      Orientamento del display <A=Automatico S=Sinistrorso D=Destrorso>",
    "      Il tasto anteriore abilita modalità \"boost\" fino a 450C durante la saldatura",
    "      Temperatura in modalità \"boost\" <C>",
    "      Visualizza il grafico della potenza assorbita al posto delle frecce",
    "      Avvia automaticamente il saldatore quando viene alimentato. <S=Modalità saldatura R=Modalità riposo N=Spento>",
    "      Durante lo spegnimento la temperatura lampeggia sul display finché la punta è calda."
  };

  const char* TempCalStatus[3] = { "Cal Temp", "Cal OK  ", "Cal Err " }; //All fixed 8 chars
  const char* UVLOWarningString = "LOW VOLT"; //Fixed width 8 chars
  const char* CoolingPromptString = "Cool"; //Fixed width 5 chars
  const char SettingTrueChar = 'S';
  const char SettingFalseChar = 'N';
  const char SettingSleepChar = 'R';
  const char SettingFastChar = 'V';
  const char SettingMediumChar = 'M';
  const char SettingSlowChar = 'L';
  const char SettingRightChar = 'D';
  const char SettingLeftChar = 'S';
  const char SettingAutoChar = 'A';
  const char SettingTempCChar = 'C';
  const char SettingTempFChar = 'F';
#endif
const char* SettingsShortNames[14] = { "PWRSC ", "STMP ", "STME ", "SHTME ",
		"MSENSE ", "TMPUNT ", "TMPRND ", "TMPSPD ", "DSPROT ", "BOOST  ",
		"BTMP ", "PWRDSP ", "ASTART ", "CLBLNK " };
