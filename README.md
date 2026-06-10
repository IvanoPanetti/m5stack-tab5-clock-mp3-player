# M5Stack Tab5 – Clock & MP3 Music Player  
*(Italiano sotto)*


---

## 🇬🇧 English Version

### **Overview**
This project turns the **M5Stack Tab5** into an **Clock with Alarm**, an **MP3 Player**, a **Web Radio Player**,
a **Wi-Fi Scanner**, and a **Notepad** without the need for any other hardware.
It was created for fun, with AI support, and may contain bugs.
Use it freely at your own risk.

The firmware folder contains the compiled firmware.bin,
ready to be loaded via a launcher.

Please note that in the event of an unexpected reboot, the audio may stop working.
If this happens, you need to completely turn off the tab by double-pressing the physical button at the bottom.
Wait 5 seconds, then press it again to turn the tab back on.
This will reset the ES8388 audio codec.

---

## 🔄 Changelog – Latest Updates
10/06/26
A notepad has been added to the main menu. Notes will be saved to the SD card in the notes folder.
If the folder isn't present, it will be created.

28/05/2026
A simple Wi-Fi scanner has been added to the Wi-Fi menu, useful for testing
the Wi-Fi signal received.

18/05/26
Fixed a bug that caused the tab to restart
when the timer expired while listening to web radio with the AAC codec.

04/05/26
Added a new item to the main menu to allow listening to web radio in .mp3 and .acc formats.

30/04/26
Added the ability to choose the number of songs .mp3 to listen to (from 0 to 9) in the Audio Menu.


24/04/26
Added in the Audio Menu a button to control the Speaker:  
If the speaker is ON, the audio will be monophonic. The sound will still be available through the headphones.
This is because the TAB5 has only one built‑in speaker.
If the speaker is OFF, the headphone audio will be stereo.

22/04/26
Added in the Audio Menu the option to set an alarm clock.

---

## 📁 **SD Card Setup**
- Copy the entire **SD** folder to the **root** of your microSD card.  
- The SD card **must be formatted as FAT32**, otherwise it will not be recognized.  
- The file **Setting.txt** contains all project settings.

### **Setting.txt format**

VariableName = Value ; Comment

The symbols `=` and `;` must be used exactly as separators.

---

## 🕒 **Analog Clock**
The analog clock display is inspired by the **Bodmer TFT_Clock** demo from the **TFT_eSPI** library.

---

## 📡 **Wi-Fi Menu**
From this menu you can select one of three Wi-Fi networks.  
These networks must be defined inside **Setting.txt** on the SD card.

Time synchronization can be done:
- manually (recommended), or  
- via Wi-Fi (unreliable, may require several attempts)

If synchronization fails, use manual input.

---

## 🎵 **Audio Menu**
- Songs must be placed inside dedicated SD folders.  
- Song filenames must follow this format:

ArtistName-SongTitle

Important:
- The `-` separator must be present.  
- No spaces near the `-` symbol.

There is no fixed limit to the number of songs.  
You can add as many as your SD card and Tab5 RAM allow.  
Tested with **over 500 songs per folder**.

### **Updating song lists**
Whenever you add or remove songs, run **Update Song List** from the menu.

---

## 🎵 **Menu WebRadio**

- Web radio URLs must be entered in the SD card's Settings.txt file and have this format:

  WebradioName = WebRadio Name, the name of the web radio that will be displayed on the screen when selected.

  WebradioURL  = WebRadio URL,      the URL of the web radio.
                                    WARNING! The audio library does not support https format. If your link
                                    supports it, remove the "s." Many radio stations also broadcast in http format:
                                    for example, transform the link
                                    https://stream.lolliradio.net/lolli_italia.mp3
                                    into
                                    http://stream.lolliradio.net/lolli_italia.mp3

WebradioCodec = audio format:       0 if the radio broadcasts with the .mp3 codec
                                    1 if the radio broadcasts with the .aac codec

---

## ⚠️ **Disclaimer**
This project was created for fun.  
I am not a programmer.  
It may contain bugs.  
Use it at your own risk.  
I am not responsible for any damage or inconvenience caused by this software.

License: **MIT**, unless otherwise specified by third‑party components.

---

## 🛠️ **Planned Features** 
calculator
  

---
---



## 🇮🇹 Versione Italiana

### **Descrizione**
Questo progetto trasforma l’**M5Stack Tab5** in un **orologio Con Allarme**  un  **lettore MP3** e un **riproduttore Webradio**
uno **Scanner wifi** e un **Blocco Note** Senza la necessita' di avere altro hardware.  
È stato creato per divertimento, con il supporto dell’AI, e può contenere bug.  
Usatelo liberamente a vostro rischio.

La cartella firmware contiene il firmware.bin già compilato
pronto per essere caricato tramite un launcher.

Si prega di notare che in caso di riavvio imprevisto, l'audio potrebbe smettere di funzionare.
In tal caso, è necessario spegnere completamente il tablet premendo due volte il pulsante fisico in basso.
Attendere 5 secondi, quindi premerlo di nuovo per riaccendere il tablet.
Questa operazione ripristinerà il codec audio ES8388.

---

## 🔄 Registro Modifiche – Ultimi Aggiornamenti
10/06/26
Inserito nel menu principale un blocco note. Gli Appunti verranno salvati sulla SD nella cartella notes.
Se la cartella non e' presente verra' creata.

28/05/26
Nel Menu wifi e' stato aggiunto un semplice wifi scanner utile per testare
il segnale wifi ricevuto.

18/05/26
Correzione Dell'errore che causava il riavvio del tab
allo scadere del timer durante l'ascolto delle webradio con codec aac

04/05/26
Aggiunta una nuova voce al menu principale per consentire l'ascolto di web radio nei formati .mp3 e .acc.

30/04/26
Aggiunto  nel Menu Audio la possibilita' di scegliere quante canzoni .mp3 ascoltare (da 0 a 9.)

24/04/26
È stato aggiunto al menu Audio un pulsante per controllare l'altoparlante:
Se l'altoparlante è acceso, l'audio sarà monofonico. Il suono sarà comunque disponibile tramite le cuffie.
Questo perché il TAB5 ha un solo altoparlante integrato.
Se l'altoparlante è spento, l'audio in cuffia sarà stereo.

22/04/26
Aggiunta nel Menu Audio la possibilità di impostare una sveglia.

---

## 📁 **Preparazione della SD**
- Copiare l’intera cartella **SD** nella **ROOT** della scheda microSD.  
- La scheda deve essere **formattata FAT32**, altrimenti non verrà riconosciuta.  
- Il file **Setting.txt** contiene tutte le impostazioni del progetto.

### **Formato di Setting.txt**

NomeVariabile = Valore ; Commento

I simboli `=` e `;` devono essere usati come separatori.

---

## 🕒 **Orologio Analogico**
La visualizzazione dell’orologio è ispirata alla demo **TFT_Clock** della libreria **TFT_eSPI** di Bodmer.

---

## 📡 **Menu Wi-Fi**
Da questo menu è possibile selezionare una delle tre reti Wi-Fi.  
Le reti devono essere inserite nel file **Setting.txt** nella root della SD.

La sincronizzazione dell’orario può essere:
- manuale (consigliata), oppure  
- tramite Wi-Fi (poco affidabile, richiede più tentativi)

In caso di problemi, usare l’inserimento manuale.

---

## 🎵 **Menu Audio**
- Le canzoni vanno inserite nelle cartelle dedicate sulla SD.   
- I nomi dei file devono essere nel formato:

NomeArtista-TitoloCanzone


Importante:
- Il simbolo `-` è obbligatorio.  
- Non devono esserci spazi vicino al trattino.

Il numero di canzoni non è definito.  
Potete inserirne quante la SD e la RAM del Tab5 permettono.  
Testato con **oltre 500 brani per cartella**.

### **Aggiornamento degli elenchi**
Dopo aver modificato le canzoni, usare l’opzione **Aggiorna Lista Canzoni**.

---

## 🎵 **Menu WebRadio**

- Gli URL delle Webradio vanno inseriti del file Setting.txt della SD e hanno questo formato:

  Nomewebradio = Nome WebRadio      nome della webradio che sara' visualizzato sul display quando selezionata.

  URLwebradio  = URL webradio       URL della webradio.  ATTENZIONE! La Libreria audio non supporta il formato https
                                    se il tuo link lo prevede, elimina la s molte radio trasmettono anche nel formato http:
                                    ad esempio trasforma il link
                                    https://stream.lolliradio.net/lolli_italia.mp3
                                    in
                                    http://stream.lolliradio.net/lolli_italia.mp3

  Codecwebradio = formato audio     0 se la radio trasmette con codec mp3
                                    1 se la radio trasmette con codec acc

---

## ⚠️ **Avvertenze**
Questo progetto è stato creato per gioco.  
Non sono un programmatore.  
Può contenere bug.  
Usatelo a vostro rischio.  
Non sono responsabile di eventuali danni o disagi causati da questo software.

Licenza: **MIT**, salvo diversa indicazione nei componenti di terze parti.

---

## 🛠️ **Prossimi Sviluppi**
Calcolatrice


---

### 📅 Updated: 10‑06‑2026  
**Author: Ivano Panetti**
