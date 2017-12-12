/*
 * Translation.c
 *
 *  Created on: 31Aug.,2017
 *      Author: Ben V. Brown
 */
#include "Translation.h"

#ifdef LANG_EN
const char* SettingsLongNames[16] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Power source. Sets cutoff voltage. <DC 10V> <S 3.3V per cell>",    //Power Source
		"Sleep Temperature <C>",    //Sleep Temp
		"Sleep Timeout <Minutes/Seconds>",    //Sleep Timeout
		"Shutdown Timeout <Minutes>",    //Shutdown Time
		"Motion Sensitivity <0.Off 1.least sensitive 9.most sensitive>",    //Motion Sensitivity
		"Temperature Unit <C=Celsius F=Fahrenheit>",    //Temp Unit
		"Display detailed information in a smaller font on the idle screen.",    //Detailed Information
		"Display Orientation <A. Automatic L. Left Handed R. Right Handed>",    //Orientation
		"Enable front key enters boost mode 450C mode when soldering",    //Boost enable
		"Temperature when in \"boost\" mode",    //Boost Temp
		"Automatically starts the iron into soldering on power up. T=Soldering, S= Sleep mode,F=Off",    //Auto start
		"Blink the temperature on the cooling screen while the tip is still hot.",    //Cooling Blink
		"Calibrate tip offset.",    //Calibrate Tip
		"Reset all settings",    //Reset Settings
		"VIN Calibration. Buttons adjust, long press to exit",    //VIN Cal
		"Display detailed information while soldering",    //ADV SLD
		};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_RU
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",    //Cell count
	"Температура Сна  <С>",//Sleep Temp
	"Переход в режим Сна <Минуты>",//Sleep timeout
	"Переходит в режим ожидания <Минуты>",//Shutdown timeout
	"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",//Sensitivity
	"В чем измерять температуру",//Temp Unit
	"Display detailed information in a smaller font on the idle screen.",//Detailed Information
	"Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",//Orientation
	"Активация  кнопки A для Турбо режима до  450С при пайке ",//Boost enable
	"Установка температуры для Турбо режима",//Boost temp
	"Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",//Auto start
	"Мигает температура на экране охлаждения, пока жало остается горячим."//Cooling blink
	"Calibrate tip offset.",//Calibrate Tip
	"Reset all settings",//Reset Settings
	"VIN Calibration. Buttons adjust, long press to exit",//VIN Cal
	"Display detailed information while soldering",//ADV SLD
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_ES
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Fuente de energía. Ajusta el límite inferior de voltaje. <DC=10V S=3.3V por celda>",
	"Temperatura en reposo. <C>",
	"Tiempo hasta activar reposo. <Minutos>",
	"Tiempo hasta apagado. <Minutos>",
	"Sensibilidad del movimiento. <0=Apagado 1=El menos sensible 9=El más sensible>",
	"Unidad de temperatura.",
	"Mostrar información detallada en el reposo.",    //Detailed Information
	"Orientación de la pantalla <A=Automático I=Mano izquierda D=Mano derecha>",
	"Activar el botón \"Boost\" en modo soldadura.",
	"Temperatura en modo \"Boost\". <C>",
	"Iniciar modo soldadura en el encendido. <V=Sí S=Modo reposo F=No>",
	"Parpadea la temperatura en el enfriamiento si la punta sigue caliente."
	"Calibrar punta.",//Calibrate Tip
	"Resetear ajustes.",//Reset Settings
	"Calibración VIN. Los botones lo ajustan. Pulsación larga para salir.",//VIN Cal
	"Mostrar información detallada mientras suelda.",//ADV SLD
};

const char* SettingsCalibrationWarning = "¡Por favor, asegúrese que la punta está a temperatura ambiente antes de continuar!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!"; 

const char SettingTrueChar = 'V'; // True in Spanish is "Verdadero"
const char SettingFalseChar = 'F'; 
const char SettingRightChar = 'D'; // Right is "Derecha"
const char SettingLeftChar = 'I'; // Left is "Izquierda"
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_SE
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Источник питания. Установка напряжения отключения. <DC 10V> <S 3.3 V на батарею>",    //Cell count
	"Температура Сна  <С>",//Sleep Temp
	"Переход в режим Сна <Минуты>",//Sleep timeout
	"Переходит в режим ожидания <Минуты>",//Shutdown timeout
	"Акселерометр <0. Выкл. 1. мин. чувствительный 9. макс. чувствительный>",//Sensitivity
	"В чем измерять температуру",//Temp Unit
	"Display detailed information in a smaller font on the idle screen.",//Detailed Information
	"Ориентация Дисплея <A. Автоматический L. Левая Рука R. Правая Рука>",//Orientation
	"Активация  кнопки A для Турбо режима до  450С при пайке ",//Boost enable
	"Установка температуры для Турбо режима",//Boost temp
	"Автоматический запуск паяльника при включении питания. T=Нагрев, S= Режим Сна,F=Выкл.",//Auto start
	"Мигает температура на экране охлаждения, пока жало остается горячим."//Cooling blink
	"Calibrate tip offset.",//Calibrate Tip
	"Reset all settings",//Reset Settings
	"VIN Calibration. Buttons adjust, long press to exit",//VIN Cal
	"Display detailed information while soldering",//ADV SLD
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_IT
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Sorgente di alimentazione; imposta il limite minimo di tensione <DC: 10V; S: 3.3V per cella>",
	"Temperatura standby <°C>",
	"Timeout standby <minuti/secondi>",
	"Timeout spegnimento <minuti>",
	"Sensibilità al movimento <0: nessuna; 1: minima; 9: massima>",
	"Unità di misura della temperatura <C: Celsius; F: Farenheit>",
	"Mostra informazioni dettagliate con un carattere più piccolo sulla schermata di inattività",
	"Orientamento del display <A: automatico; S: mano sinistra; D: mano destra>",
	"Il tasto anteriore attiva la modalità \"boost\" durante la saldatura",
	"Temperatura in modalità \"boost\"",
	"Attiva automaticamente il saldatore quando viene alimentato <A: saldatura; S: standby; D: disattiva>",
	"Durante il raffreddamento mostra la temperatura sul display se la punta è ancora calda"
	"Calibra l'offset della punta",//Calibrate Tip
	"Ripristina tutte le impostazioni",//Reset Settings
	"Calibra la tensione in entrata; regola con i bottoni, tieni permuto per uscire",//VIN Cal
	"Mostra informazioni dettagliate mentre stai saldando",//ADV SLD
};

const char* SettingsCalibrationWarning = "Assicurati che la punta si trovi a temperatura ambiente prima di continuare!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Standby";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "ATTENZIONE! PUNTA CALDA!";

const char SettingTrueChar = 'A';
const char SettingFalseChar = 'D';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'S';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_FR
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Type d\'alimentation. Regle la tension de coupure. <DC=10V S=3.3V par cellules>",
	"Temperature en veille. <C>",
	"Temps avant mise en veille. <Minutes>",
	"Temps avant extinction. <Minutes>",
	"Sensibilitee du capteur de mouvement. <0=Inactif 1=Peu sensible 9=Tres sensible>",
	"Unitee de temperature.",
	"Afficher des informations detaillees en petit lors de la veille",    //Detailed Information
	"Orientation de l\'affichage. <A=Automatique G=Gaucher D=Droitier>",
	"Active le mode \"Boost\" 450C sur le bouton de devant pendant la soudure.",
	"Temperature du mode \"Boost\". <C>",
	"Demarre automatiquement la soudure a l\'allumage. <A=Active, V=Mode Veille, D=Desactive>",
	"Fait clignotter la temperature lors du refroidissement pendant que la panne est chaude."
	"Compenser l\'erreur de la panne",//Calibrate Tip
	"Reinitialiser tout les reglages",//Reset Settings
	"Calibration VIN. Boutons pour ajuster, appui long pour quitter",//VIN Cal
	"Afficher des informations detaillees pendant la soudure",//ADV SLD
};

const char* SettingsCalibrationWarning = "Please ensure the tip is at room temperature before continuing!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";// Must be <= 4 chars
const char* SleepingAdvancedString = "Sleeping...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "WARNING! TIP HOT!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_DE
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Spannungsquelle (Abschaltspannung) <DC=10V, nS=n*3.3V für n LiIon-Zellen>",
	"Ruhetemperatur (In der eingestellten Einheit)",
	"Ruhemodus nach <Sekunden/Minuten>",
	"Abschaltzeit <Minuten>",
	"Bewegungsempfindlichkeit <0=Aus, 1=Minimal ... 9=Maximal>",
	"Temperatureinheit <C=Celsius, F=Fahrenheit>",
	"Detaillierte Anzeige im Ruhemodus <T=An, F=Aus>",    //Detailed Information
	"Ausrichtung der Anzeige <A=Auto, L=Linkshändig, R=Rechtshändig>",
	"Vordere Taste für Temperaturboost verwenden <T=An, F=Aus>",
	"Temperatur im Boostmodus  (In der eingestellten Einheit)",
	"Automatischer Start des Lötmodus beim Einschalten der Spannungsversorgung. <T=An, F=Aus>",
	"Blinkende Temperaturanzeige beim Abkühlen, solange heiß. <T=An, F=Aus>"
	"Kalibrierung der Lötspitzentemperatur",//Calibrate Tip
	"Alle Einstellungen zurücksetzen",//Reset Settings
	"Kalibrierung der Eingangsspannung. Kurzer Tastendruck zum Einstellen, langer Tastendruck zum Verlassen.",//VIN Cal
	"Detaillierte Anzeige im Lötmodus <T=An, F=Aus>",//ADV SLD
};

const char* SettingsCalibrationWarning = "Vor dem Fortfahren muss die Lötspitze vollständig abgekühlt sein!";
const char* UVLOWarningString = "LOW VOLT";//Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";// Must be <= 4 chars
const char* SleepingAdvancedString = "Ruhemodus...";// <=17 chars
const char* WarningSimpleString = "HEIß";//Must be <= 4 chars
const char* WarningAdvancedString = "Achtung! Spitze Heiß!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_SK
const char* SettingsLongNames[16] = {
	/*These are all the help text for all the settings.*/
	/*No requirements on spacing or length*/
	"Zdroj napatia. Nastavit napatie pre vypnutie (cutoff)  <DC=10V, nS=n*3.3V pre LiIon clanky>",
	"Kludova teplota (v nastavenych jednotkach)",
	"Kludovy rezim po <sekundach/minutach>",
	"Cas na vypnutie <minuty>",
	"Citlivost detekcie pohybu <0=Vyp, 1=Min ... 9=Max>",
	"Jednotky merania teploty <C=stupne Celzia, F=stupne Fahrenheita>",
	"Zobrazit detailne informacie v kludovom rezime <T=Zap, F=Vyp>",    //Detailed Information
	"Orientacia displeja <A=Auto, L=Lavak, R=Pravak>",
	"Povolit tlacidlo pre prudky nahrev <T=Zap, F=Vyp>",
	"Cielova teplota pre prudky nahrev (v nastavenych jednotkach)",
	"Pri starte spustit rezim spajkovania <T=Zap, F=Vyp, S=Spanok>",
	"Blikanie ukazovatela teploty pocas chladnutia hrotu <T=Zap, F=Vyp>"
	"Kalibracia posunu hrotu",//Calibrate Tip
	"Tovarenske nastavenia",//Reset Settings
	"Kalibracia VIN. Kratke stlacenie meni nastavenie, dlhe stlacenie pre navrat",//VIN Cal
	"Zobrazenie detailov pocas spajkovania <T=Zap, F=Vyp>",//ADV SLD
};

const char* SettingsCalibrationWarning = "Najprv sa prosim uistite, ze hrot ma izbovu teplotu!";
const char* UVLOWarningString = "LOW VOLT";//Fixed width 8 chars
const char* SleepingSimpleString = "Chrr";// Must be <= 4 chars
const char* SleepingAdvancedString = "Kludovy rezim...";// <=17 chars
const char* WarningSimpleString = "HOT!";//Must be <= 4 chars
const char* WarningAdvancedString = "Pozor! Hrot je horuci!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_TR
const char* SettingsLongNames[16] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Güç Kaynağı. kesim geriliminı ayarlar. <DC 10V> <S 3.3V hücre başına>",    //Power Source
		"Uyku Sıcaklığı <C>",    //Sleep Temp
		"Uyku Zaman Aşımı <Dakika/Saniye>",    //Sleep Timeout
		"Kapatma Zaman Aşımı <Dakika>",    //Shutdown Time
		"Hareket Hassasiyeti <0.Kapalı 1.En az duyarlı 9.En duyarlı>",    //Motion Sensitivity
		"Sıcaklık Ünitesi <C=Celsius F=Fahrenheit>",    //Temp Unit
		"Boş ekranda ayrıntılı bilgileri daha küçük bir yazı tipi ile göster.",    //Detailed Information
		"Görüntü Yönlendirme <A. Otomatik L. Solak R. Sağlak>",    //Orientation
		"Lehimleme yaparken ön tuşa basmak Boost moduna sokar(450C)",    //Boost enable
		"\"boost\" Modu Derecesi",    //Boost Temp
		"Güç verildiğinde otomatik olarak lehimleme modunda başlat. T=Lehimleme Modu, S= Uyku Modu,F=Kapalı",    //Auto start
		"Soğutma ekranında uç hala sıcakken derece yanıp sönsün.",    //Cooling Blink
		"Ucu kalibre et.",    //Calibrate Tip
		"Bütün ayarları sıfırla",    //Reset Settings
		"VIN Kalibrasyonu. Düğmeler ayarlar, çıkmak için uzun bas.",    //VIN Cal
		"Lehimleme yaparken detaylı bilgi göster",    //ADV SLD
		};

const char* SettingsCalibrationWarning = "Lütfen devam etmeden önce ucun oda sıcaklığında olduğunu garantiye alın!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Uyuyor...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "UYARI! UÇ SICAK!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_HR
const char* SettingsLongNames[16] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Izvor napajanja. Postavlja napon iskljucivanja. <DC 10V> <S 3.3V po celiji>",    //Power Source
		"Temperatura spavanja. <C>",    //Sleep Temp
		"Vrijeme spavanja. <Minute/Sekunde>",    //Sleep Timeout
		"Vrijeme gasenja. <Minutes>",    //Shutdown Time
		"Osjetljivost prepoznavanja pokreta. <0=Ugaseno, 1=Najmanje osjetljivo, 9=Najosjetljivije>",    //Motion Sensitivity
		"Jedinica temperature. <C=Celzij, F=Fahrenheit>",    //Temp Unit
		"Prikazivanje detaljnih informacija manjim fontom tijekom cekanja.",    //Detailed Information
		"Orijentacija ekrana. <A=Automatski, L=Ljevoruki, D=Desnoruki>",    //Orientation
		"Prednji gumb prilikom lemljenja aktivira pojacani (Boost) nacin.",    //Boost enable
		"Temperatura u pojacanom (Boost) nacinu.",    //Boost Temp
		"Nacin u kojemu se lemilica pokrece po ukljucivanju napajanja. <+=Lemljenje, S=Spavanje, -=Ugaseno>",    //Auto start
		"Bljeskanje temperature prilikom hladjenja, ako je lemilica vruca.",    //Cooling Blink
		"Kalibriranje temperature vrha.",    //Calibrate Tip
		"Vracanje svih postavki.",    //Reset Settings
		"Kalibracija ulaznog napona. Podesavanje gumbima, dugacki pritisak za kraj.",    //VIN Cal
		"Prikazivanje detaljnih informacija tijekom lemljenja.",    //ADV SLD
		};

const char* SettingsCalibrationWarning = "Provjerite da je vrh ohladjen na sobnu temperaturu prije nego sto nastavite!";
const char* UVLOWarningString = "NAPON!!!";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Spavanje...";    // <=17 chars
const char* WarningSimpleString = "VRUC";    //Must be <= 4 chars
const char* WarningAdvancedString = "OPREZ! Vrsak je vruc!";

const char SettingTrueChar = '+';
const char SettingFalseChar = '-';
const char SettingRightChar = 'D';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif

#ifdef LANG_CS_CZ
const char* SettingsLongNames[16] = {
		/*These are all the help text for all the settings.*/
		/*No requirements on spacing or length*/
		"Zdroj napajeni. Pri nizsim napeti se odpoji <DC=10V, xS=x*3.3V pro LiPo,LiIon...>",    //Power Source
		"Teplota v rezimu spanku",    //Sleep Temp
		"Cas do rezimu spanku <Minut/Sekund>",    //Sleep Timeout
		"Cas do automatickeho vypnuti <Minut>",    //Shutdown Time
		"Citlivost detekce pohybu <0=Vyp, 1=Min, ... 9=Max>",    //Motion Sensitivity
		"Jednotky mereni teploty <C=Celsius, F=Fahrenheit>",    //Temp Unit
		"Zobrazit podrobnosti na vychozi obrazovce <Z=Zap, V=Vyp>",    //Detailed Information
		"Otoceni displaye <A=Auto, L=Levak, P=Pravak>",    //Orientation
		"Povolit boost drzenim leveho tlacitka pri pajeni <Z=Zap, V=Vyp>",    //Boost enable
		"Teplota pri boostu",    //Boost Temp
		"Pri startu ihned nahrivat hrot <Z=Zap, V=Vyp, S=Rezim spanku>",    //Auto start
		"Blikani teploty pri chladnuti, dokud je hrot horky <Z=Zap, V=Vyp>",    //Cooling Blink
		"Kalibrovat mereni teploty",    //Calibrate Tip
		"Obnovit tovarni nastaveni",    //Reset Settings
		"Kalibrovat vstupni napeti. Tlacitky upravte, podrzenim potvrdte.",    //VIN Cal
		"Zobrazit podrobnosti pri pajeni <Z=Zap, V=Vyp>",    //ADV SLD
		};

const char* SettingsCalibrationWarning = "Ujistete se, ze hrot ma pokojovou teplotu! "; //ending space needed
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzz ";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Rezim spanku...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "!! HORKY HROT !!";   // <= 16 chars

const char SettingTrueChar = 'Z';
const char SettingFalseChar = 'V';
const char SettingRightChar = 'P';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
#ifdef LANG_HUN
const char* SettingsLongNames[16] = {
/*These are all the help text for all the settings.*/
/*No requirements on spacing or length*/
"Áramforrás. Beállítja a lekapcsolási feszültséget. <DC 10V> <S 3.3V cellánként>",    //Power Source
		"Alvási hőmérséklet <C>",    //Sleep Temp
		"Elalvási időzítő <Perc/Másodperc>",    //Sleep Timeout
		"Kikapcsolási időzítő <Minutes>",    //Shutdown Time
		"Mozgás érzékenység beállítása. <0.Ki 1.kevésbé érzékeny 9.legérzékenyebb>",    //Motion Sensitivity
		"Hőmérsékleti egység <C=Celsius F=Fahrenheit>",    //Temp Unit
		"Részletes információ megjelenítése kisebb betűméretben a készenléti képernyőn.",    //Detailed Information
		"Megjelenítési tájolás <A. Automatikus L. Balkezes R. Jobbkezes>",    //Orientation
		"Elülső gombbal lépjen boost módba, 450C forrasztás közben",    //Boost enable
		"Hőmérséklet \"boost\" módban",    //Boost Temp
		"Bekapcsolás után automatikusan lépjen forrasztás módba. T=Forrasztás, S=Alvó mód,F=Ki",    //Auto start
		"Villogjon a hőmérséklet hűlés közben, amíg a hegy forró.",    //Cooling Blink
		"Hegy hőmérsékletének kalibrálása",    //Calibrate Tip
		"Beállítások alaphelyzetbe állítása",    //Reset Settings
		"A bemeneti feszültség kalibrálása. Röviden megnyomva állítsa be, hosszan nyomja meg a kilépéshez.",    //VIN Cal
		"Részletes információk megjelenítése forrasztás közben",    //ADV SLD
		};

const char* SettingsCalibrationWarning = "Folytatás előtt győződj meg róla, hogy a hegy szobahőmérsékletű!";
const char* UVLOWarningString = "LOW VOLT";    //Fixed width 8 chars
const char* SleepingSimpleString = "Zzzz";    // Must be <= 4 chars
const char* SleepingAdvancedString = "Alvás...";    // <=17 chars
const char* WarningSimpleString = "HOT!";    //Must be <= 4 chars
const char* WarningAdvancedString = "FIGYELEM! FORRÓ HEGY!";

const char SettingTrueChar = 'T';
const char SettingFalseChar = 'F';
const char SettingRightChar = 'R';
const char SettingLeftChar = 'L';
const char SettingAutoChar = 'A';
#endif
//Currently the settings names are not translated
const char* SettingsShortNames[16] = { /**/
"PWRSC ",    // Power Source (DC or batt)
		"STMP ", 	// Sleep Temperature
		"STME ", 	// Sleep Timeout
		"SHTME ",    // Shutdown Temperature
		"MSENSE ",    // Motion sensitivity level
		"TMPUNT ",			//Temp in F and C
		"ADVIDL ",    // Advanced idle display mode enable
		"DSPROT ",    // Display rotation mode
		"BOOST  ",    // Boost enabled
		"BTMP ", 	// Boost temperature
		"ASTART ",    // Automatic Start mode
		"CLBLNK ",    // Cooldown blink
		"TMP CAL?",    // Temperature calibration enter menu
		"RESET? ",    // Settings reset command
		"CAL VIN?", "ADVSLD ",    //advanced soldering screens
		};
