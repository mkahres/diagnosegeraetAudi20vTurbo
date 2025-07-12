import serial
from serial.tools import list_ports
import csv
import matplotlib.pyplot as plt
from matplotlib.animation import FuncAnimation
from collections import deque
from tkinter import *
from tkinter import filedialog, ttk
import os
from datetime import datetime
import subprocess
import mplcursors
import threading
import time
from tkinter import StringVar
from matplotlib.backends.backend_tkagg import FigureCanvasTkAgg
from tkinter import messagebox

# -------- Konfiguration --------
werte_bereich = [
    (184, 215), (20, 25), (77, 83), (121, 135), (80, 125),
    (123, 137), (41, 55), (123, 133), (115, 141), (35, 37)
]
anzeigebereich = [
    (80, 250), (10, 45), (60, 120), (80, 180), (50, 200),
    (80, 180), (20, 80), (90, 170), (50, 200), (25, 50)
]
bezeichnungen = [
    "Motortemperatur", "Motorlast", "Drehzahl", "LL-Stab. Arbeitsber.",
    "LL-Stab. Nullpunkt", "LL-Stab. Lastanpassung", "LL-Stab. Kennl.-Strg.", "Lambdaregelung aktiv",
    "Lambdareg. Anpassung", "Zündwinkel"
]
bezeichnungen_lang = [
    "Motortemperatur (184-215 => 85-105°C)", "Motorlast (20-25)", "Drehzahl (77-83 => 770-830 U/min)", "Leerlaufstabilisierung Arbeitsbereich (121-135)",
    "Leerlaufstabilisierung Nullpunkt (80-125)", "Leerlaufstabilisierung Lastanpassung (123-137)", "Leerlaufstabilisierung Kennliniensteuerung (41-55)", 
    "Lambdaregelung aktiv (123-133 - nach 1,5 Min.)", "Lambdaregelung Anpassung (115-141)", "Zündwinkel (35-37 => 8-12° v.OT)"
]
max_points = 60

# -------- Zustandsvariablen --------
daten = [deque(maxlen=max_points) for _ in range(10)]
zeiten = deque(maxlen=max_points)
csv_daten = []
csv_dateiname = ""
ser = None
ani = None
frame_index = 0
linien = []
linien_gruppe = []
fig = None
axs = []
canvas = None
daten_puffer = deque(maxlen=300)
verbunden = False
start_aktiv = False
lese_thread_aktiv = False
werte = [0] * 10

# -------- GUI-Elemente --------
csv_slider = None
csv_slider_label = None
werte_labels = []

def verbinden():
    global ser, verbunden, start_aktiv, lese_thread_aktiv

    info_label.config(text="")

    if ser and ser.is_open:
        try:
            ser.close()
        except:
            pass

    try:
        port = get_selected_device()
        print(port)
        if port is None:
            meldung_zeigen("⚠️ Kein COM-Port ausgewählt", dauer=0)
            return

        print(f"🔌 Verbinde mit {port}...")
        ser = serial.Serial(port, 115200, timeout=2)
        time.sleep(2)  # warten auf automatischen Reset des Arduinos

        # Auf "READY" warten
        startzeit = time.time()
        empfangspuffer = ""
        bereit = False

        while time.time() - startzeit < 5:  # max. 5 Sekunden warten
            while ser.in_waiting:
                zeichen = ser.read().decode('utf-8', errors='ignore')
                empfangspuffer += zeichen
                if "READY" in empfangspuffer:
                    ser.write(b"<H>")
                    bereit = True
                    break
            if bereit:
                break

        if bereit:
            time.sleep(4)
            meldung_zeigen("✅ Verbunden und Arduino bereit", farbe="black")
            verbunden = True

            # Buttons aktivieren/deaktivieren
            button_verbinden.config(state="disabled")
            button_start.config(state="normal")
            button_stop.config(state="disabled")
        else:
            meldung_zeigen("⚠️ Keine Antwort vom Arduino (READY fehlt)", dauer=0)
            ser.close()
            verbunden = False

    except Exception as e:
        meldung_zeigen(f"❌ Verbindungsfehler: {e}", dauer=0)
        verbunden = False


def lesen_bis_ok(ser, erwartete_antwort, timeout=5):
    """Liest serielle Zeilen bis erwartete_antwort oder Timeout."""
    import time
    start_time = time.time()
    while True:
        if time.time() - start_time > timeout:
            print(f"❌ Timeout: '{erwartete_antwort}' nicht erhalten.")
            return False
        if ser.in_waiting > 0:
            zeile = ser.readline().decode(errors='ignore').strip()
            print(f"RX: {zeile}")
            if zeile == erwartete_antwort:
                return True
            
def start_datenempfang():
    global lese_thread_aktiv, start_aktiv, verbunden, modus
    if not verbunden:
        print("⚠️ Keine Verbindung")
        return
    try:
        ser.write(b"<S>")
        print("▶️ Startsignal gesendet, warte auf 'START OK'...")
        if lesen_bis_ok(ser, "START OK", timeout=10):
            lese_thread_aktiv = True
            threading.Thread(target=daten_lesen_thread, daemon=True).start()
            print("▶️ Datenempfang gestartet")
            start_aktiv = True
            start_livedaten()
            button_start.config(state="disabled")
            button_stop.config(state="normal")
        else:
            meldung_zeigen("❌ Start OK nicht erhalten, Start abgebrochen.")
    except Exception as e:
        meldung_zeigen(f"❌ Fehler beim Start: {e}")

def stop_datenempfang():
    try:
        ser.write(b"<X>")
        print("⏹ Stop-Signal gesendet, warte auf 'STOP OK'...")
    except Exception as e:
        meldung_zeigen(f"❌ Fehler beim Stoppen: {e}")

def intervall_aendern(event=None):
    global ani
    if ani is not None:
        try:
            neu = int(intervall_var.get())
            ani.event_source.stop()
            ani._stop()
            ani = FuncAnimation(fig, update, interval=neu, cache_frame_data=False)
            canvas.draw_idle()
            print(f"✅ Intervall auf {neu} ms geändert")
        except Exception as e:
            print("❌ Fehler beim Setzen des Intervalls:", e)

# COM-Ports laden
def lade_com_ports():
    ports = list_ports.comports()
    # port_list: Liste von (device, description)
    port_list = [(port.device, port.description) for port in ports]
    return port_list

def refresh_com_ports():
    global anzeige_geraet_liste
    port_list = lade_com_ports()
    anzeige_geraet_liste = [(format_entry(dev, desc), dev) for dev, desc in port_list]

    menu = com_port_optionmenu["menu"]
    menu.delete(0, "end")

    for anzeige, _ in anzeige_geraet_liste:
        menu.add_command(label=anzeige, command=lambda v=anzeige: com_port_var.set(v))

# Anzeige-String bauen
def format_entry(device, description):
    short_desc = (description[:7] + "...") if len(description) > 7 else description
    return f"{short_desc} ({device})"

def get_selected_device():
    selected_anzeige = com_port_var.get()
    for anzeige, device in anzeige_geraet_liste:
        if anzeige == selected_anzeige:
            return device
    return None

def daten_lesen_thread():
    global frame_index, lese_thread_aktiv, start_aktiv, werte
    puffer = ""

    while lese_thread_aktiv:
        try:
            if ser.in_waiting:               
                puffer += ser.read(ser.in_waiting).decode(errors="ignore")
                while "\n" in puffer:
                    zeile, puffer = puffer.split("\n", 1)
                    zeile = zeile.strip()

                    if not zeile:
                        continue

                    if zeile == "STOP OK":
                        lese_thread_aktiv = False
                        start_aktiv = False
                        print("⏹ Datenempfang gestoppt")
                        button_start.config(state="normal")
                        button_stop.config(state="disabled")
                        continue
                    teile = zeile.split(",")
                    if len(teile) != 10:
                        print(f"⚠️ Ungültige Zeile empfangen (falsche Anzahl Werte): {zeile}")
                        continue

                    try:
                        werte = list(map(int, teile))
                        daten_puffer.append((frame_index, werte))
                        frame_index += 1
                    except ValueError:
                        print(f"⚠️ Fehler beim Parsen (nicht-int-Wert): {zeile}")
                        continue

        except Exception as e:
            print(f"❌ Fehler im Lesethread: {e}")
            time.sleep(0.1)  # Falls was schiefgeht, kurz warten und weiter


# -------- Datenverarbeitung --------
def update(frame):
    global frame_index, werte

    if modus == "csv":
        return linien  # CSV-Modus: keine Animation

    if modus == "live":
        while daten_puffer:
            index, werte = daten_puffer.popleft()
            zeiten.append(index)

            for i in range(10):
                daten[i].append(werte[i])

            try:
                with open(csv_dateiname_var.get(), mode='a', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow([index + 1] + werte)
            except FileNotFoundError:
                pass  # optional: Fehlerbehandlung/logging

        # Werte-Labels aktualisieren mit den neuesten Daten
        if werte is None or len(werte) < 10:
            return
        for i in range(10):
            wert = werte[i]
            werte_labels[i].config(text=f"{bezeichnungen[i]}: {wert:<3}", anchor='w')

    # ---- Darstellung aktualisieren ----
    if ansicht_var.get() == "gruppe":
        for i, linie in enumerate(linien_gruppe):
            if sichtbar_vars[i].get():
                linie.set_data(range(len(daten[i])), daten[i])
            else:
                linie.set_data([], [])

    else:
        # Jeder Plot einzeln
        for i in range(10):
            linien[i].set_data(range(len(daten[i])), daten[i])
            axs[i].set_xlim(0, max_points)

    canvas.draw_idle()
    return linien


def csv_slider_update(val):
    global werte_labels

    index = int(val)
    window = max_points
    halbfenster = window // 2

    if len(csv_daten) == 0:
        csv_slider_label.config(text="CSV Datei ist leer")
        for i in range(10):
            werte_labels[i].config(text=f"{bezeichnungen[i]}: -", anchor='w')
        return

    start = max(0, index - halbfenster)
    end = min(len(csv_daten), start + window)
    if end - start < window and start > 0:
        start = max(0, end - window)  # versuche nach links auszugleichen

    zeitfenster = [int(row[0]) for row in csv_daten[start:end]]

    if not zeitfenster:
        return  # Keine Daten zum Anzeigen

    ansicht = ansicht_var.get()

    if ansicht == "einzeln":
        for i in range(10):
            daten[i].clear()
            wertefenster = [float(row[i + 1]) for row in csv_daten[start:end]]
            daten[i].extend(wertefenster)
            linien[i].set_data(zeitfenster, daten[i])
            axs[i].set_xlim(zeitfenster[0], zeitfenster[-1])

    else:  # Gruppenansicht
        for i in range(10):
            daten[i].clear()
            wertefenster = [float(row[i + 1]) for row in csv_daten[start:end]]
            daten[i].extend(wertefenster)
            linien[i].set_data(zeitfenster, daten[i])
            linien[i].set_visible(sichtbar_vars[i].get())

        axs[0].set_xlim(zeitfenster[0], zeitfenster[-1])

    # Label aktualisieren
    csv_slider_label.config(text=f"Wert Nr. {index + 1} von {len(csv_daten)}")
    canvas.draw_idle()

    # Werte aktualisieren
    aktuelle_zeile = csv_daten[index]
    for i in range(10):
        wert = aktuelle_zeile[i + 1]
        werte_labels[i].config(text=f"{bezeichnungen[i]}: {wert:<3}", anchor='w')


# -------- Plot --------
def plot_starten():
    global linien, linien_gruppe, ani, fig, axs, canvas, max_points, daten, zeiten

    max_points = max_points_var.get()

    if not daten:
        daten = [deque(maxlen=max_points) for _ in range(10)]
        zeiten = deque(maxlen=max_points)
    else:
        daten = [deque(dq, maxlen=max_points) for dq in daten]
        zeiten = deque(zeiten, maxlen=max_points)

    if canvas is not None:
        canvas.get_tk_widget().destroy()
        try:
            plt.close(fig)
        except:
            pass

    skalierung = skalierung_var.get()
    ansicht = ansicht_var.get()

    # ---- Gruppenansicht ----
    if ansicht == "gruppe":
        fig, ax = plt.subplots(figsize=(12, 6))
        axs = [ax]
        linien = []
        linien_gruppe = []

        y_min, y_max = 0, 256  # feste Skalierung

        for i in range(10):
            sichtbarkeit = sichtbar_vars[i].get()
            linie, = ax.plot([], [], label=bezeichnungen[i], visible=sichtbarkeit)
            linien.append(linie)
            linien_gruppe.append(linie)

        ax.set_title("Gruppenansicht aller Sensoren", fontsize=12)
        ax.set_xlim(0, max_points)
        ax.set_ylim(y_min, y_max)
        legende_aktualisieren()
        mplcursors.cursor(linien, hover=True)
        fig.subplots_adjust(top=0.95, bottom=0.1)

        # Checkboxen aktivieren
        for cb in checkbox_frame.winfo_children():
            cb.config(state="normal")

        # Skalierung deaktivieren
        for btn in skalierung_buttons:
            btn.config(state="disabled")

    # ---- Einzeldarstellung ----
    else:
        fig, axs = plt.subplots(5, 2, figsize=(12, 12))
        axs = axs.flatten()
        linien = []
        linien_gruppe = []  # leeren

        for i in range(10):
            min_val, max_val = werte_bereich[i]

            if skalierung == "standard":
                y_min, y_max = anzeigebereich[i]
            else:
                y_min, y_max = 0, 256

            linie, = axs[i].plot([], [], label=bezeichnungen[i])
            axs[i].set_title(bezeichnungen_lang[i], fontsize=10)
            axs[i].set_xlim(0, max_points)
            axs[i].set_ylim(y_min, y_max)
            axs[i].axhspan(min_val, max_val, color='green', alpha=0.1)
            axs[i].legend(loc="upper right", fontsize='small')
            linien.append(linie)

        mplcursors.cursor(linien, hover=True)
        fig.subplots_adjust(top=0.97, bottom=0.05, hspace=0.6)

        # Checkboxen deaktivieren
        for cb in checkbox_frame.winfo_children():
            cb.config(state="disabled")

        # Skalierung aktivieren
        for btn in skalierung_buttons:
            btn.config(state="normal")

    for widget in plot_frame.winfo_children():
        widget.destroy()

    canvas = FigureCanvasTkAgg(fig, master=plot_frame)
    canvas.get_tk_widget().pack(fill=BOTH, expand=1)

    if modus == "live":
        interval = int(intervall_var.get())
        
        if ani and hasattr(ani, 'event_source') and ani.event_source:
            ani.event_source.stop()
            ani = None
        
        ani = FuncAnimation(fig, update, interval=interval, cache_frame_data=False)

    canvas.draw()

    # CSV-Modus: Nach Neuzeichnen Daten aktualisieren    
    if modus == "csv" and csv_daten:
        index = int(csv_slider.get())
        csv_slider_update(index)



def leeren_plot_initialisieren():
    global fig, axs, canvas

    try:
        plt.close(fig)
    except:
        pass

    fig, axs = plt.subplots(5, 2, figsize=(12, 12))
    axs = axs.flatten()

    for i, ax in enumerate(axs):
        ax.set_title(bezeichnungen_lang[i], fontsize=10)
        ax.set_xlim(0, max_points)
        ax.set_ylim(*anzeigebereich[i])
        ax.axhspan(*werte_bereich[i], color='green', alpha=0.1)

    fig.subplots_adjust(top=0.97, bottom=0.05, hspace=0.6)

    for widget in plot_frame.winfo_children():
        widget.destroy()

    canvas = FigureCanvasTkAgg(fig, master=plot_frame)
    canvas.get_tk_widget().pack(side=RIGHT, fill=BOTH, expand=True)
    canvas.draw()

    if ansicht_var.get() == "einzeln":
        for cb in checkbox_frame.winfo_children():
            cb.config(state="disabled")
        for btn in skalierung_buttons:
            btn.config(state="normal")
    else:
        for cb in checkbox_frame.winfo_children():
            cb.config(state="normal")
        for btn in skalierung_buttons:
            btn.config(state="disabled")

def ansicht_umschalten():
    if ansicht_var.get() == "gruppe":
        # Skalierung deaktivieren
        for btn in skalierung_buttons:
            btn.config(state="disabled")
    else:
        # Skalierung aktivieren
        for btn in skalierung_buttons:
            btn.config(state="normal")
    plot_starten()

def legende_aktualisieren():
    ax = linien_gruppe[0].axes
    ax.legend(
        [linie for linie in linien_gruppe if linie.get_visible()],
        [bezeichnungen[i] for i, linie in enumerate(linien_gruppe) if linie.get_visible()],
        loc="upper right", fontsize='small'
    )

def checkbox_toggle(index):
    if ansicht_var.get() == "gruppe" and linien_gruppe:
        linien_gruppe[index].set_visible(sichtbar_vars[index].get())
        legende_aktualisieren()
        canvas.draw_idle()

def meldung_zeigen(text, dauer=5000, farbe="red"):
    print(text)
    info_label.config(text=text, fg=farbe)
    if dauer:
        fenster.after(dauer, lambda: info_label.config(text=""))

def start_livedaten():
    global csv_dateiname, daten, zeiten, frame_index, modus

    modus = "live"
    daten = [deque(maxlen=max_points) for _ in range(10)]
    zeiten.clear()
    frame_index = 0
    
    timestamp = datetime.now().strftime("%Y-%m-%d_%H%M%S")
    csv_dateiname_neu = f"sensordaten_{timestamp}.csv"
    csv_dateiname_label.config(text=f"Aktive Datei: {csv_dateiname_neu}")
    csv_dateiname_var.set(csv_dateiname_neu)

    with open(csv_dateiname_neu, mode='w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(["Zeit"] + bezeichnungen)

    if csv_slider:
        csv_slider.pack_forget()
        csv_slider_label.pack_forget()

        plot_starten()

def csv_laden():
    global csv_daten, csv_dateiname, ser, daten, zeiten, frame_index, lese_thread_aktiv, modus

    if lese_thread_aktiv:
        meldung_zeigen("Bitte erst Live-Übertragung beenden")
        return
    info_label.config(text="")
    modus = "csv"
    daten = [deque(maxlen=max_points) for _ in range(10)]
    zeiten.clear()
    frame_index = 0

    dateipfad = filedialog.askopenfilename(filetypes=[("CSV-Dateien", "*.csv")])
    if not dateipfad:
        return
    with open(dateipfad, newline='') as f:
        reader = csv.reader(f)
        next(reader)
        csv_daten = [row for row in reader]
    csv_dateiname_label.config(text=f"Wiedergabe: {os.path.basename(dateipfad)}")
    csv_dateiname_var.set(dateipfad)

    if len(csv_daten) == 0:
        csv_slider_label.config(text="CSV-Datei ist leer")
        csv_slider.pack_forget()
        csv_slider_label.pack()
        plot_starten()
        return
    
    if csv_slider:
        csv_slider.config(to=len(csv_daten) - 1, showvalue=False)
        standard_index = min(0, len(csv_daten) - 1)  # Nicht über das Ende hinausgehen
        csv_slider.set(standard_index)
        plot_starten() 
        csv_slider_update(standard_index)
        csv_slider.pack()
        csv_slider_label.config(text="Scroll durch CSV")
        csv_slider_label.pack()

# -------- Explorer öffnen --------
def ordner_öffnen():
    pfad = os.path.abspath(csv_dateiname_var.get())
    if os.path.exists(pfad):
        subprocess.Popen(f'explorer /select,"{pfad}"')

def zeige_ueber_info():
    messagebox.showinfo(
        "Über dieses Tool",
        "Diagnose-Tool für Audis mit 20V Turbo-Motoren mit Bosch Motronic (3B, AAN, etc.)\n\nEntwickelt von Cyberdinosaurus Systems\n\n© 2025"
    )



# -------- GUI --------
fenster = Tk()
fenster.title("Diagnose-Tool - Audi 20V Turbo mit Bosch Motronic (3B, AAN, etc.) - V1.0")
fenster.state('zoomed')

# -------- Variablen --------
csv_dateiname_var = StringVar()
skalierung_var = StringVar(value="standard") 
skalierung_buttons = []
com_port_var = StringVar()
ansicht_var = StringVar(value="einzeln")
sichtbar_vars = [BooleanVar(value=True) for _ in range(10)]
max_points_var = IntVar(value=60)
intervall_var = StringVar(value="200")

# -------- Hauptlayout --------
menu_frame = Frame(fenster, width=250)
menu_frame.pack(side=LEFT, fill=Y, padx=10, pady=10)
menu_frame.pack_propagate(False)

plot_frame = Frame(fenster)
plot_frame.pack(side=LEFT, fill=BOTH, expand=True)

# Rechter Bereich für Anzeigeoptionen und Checkboxen
rechte_seite_frame = Frame(fenster)
rechte_seite_frame.pack(side=RIGHT, fill=Y, padx=10, pady=10)

# -------- Anzeigeoptionen (rechts oben, vertikal) --------
anzeigeoptionen_oben = Frame(rechte_seite_frame)
anzeigeoptionen_oben.pack(side=TOP, fill=X)

# Skalierung
skalierung_frame = LabelFrame(anzeigeoptionen_oben, text="Skalierung")
skalierung_frame.pack(fill="x", pady=(0, 5))
btn1 = Radiobutton(skalierung_frame, text="Standard", variable=skalierung_var, value="standard")
btn1.pack(anchor="w")
skalierung_buttons.append(btn1)
btn2 = Radiobutton(skalierung_frame, text="0–255", variable=skalierung_var, value="zero_255")
btn2.pack(anchor="w")
skalierung_buttons.append(btn2)

# Anzahl Werte
anzahl_frame = LabelFrame(anzeigeoptionen_oben, text="Anzahl Werte")
anzahl_frame.pack(fill="x", pady=(0, 5))
for val, label in [(10, "10 (ca. 2 Sek.)"), (25, "25 (ca. 5 Sek.)"), (60, "60 (ca. 12 Sek.)"), (300, "300 (ca. 1 Min.)"), (1500, "1500 (ca. 5 Min.)")]:
    Radiobutton(anzahl_frame, text=label, variable=max_points_var, value=val).pack(anchor="w")

# Ansicht
ansicht_frame = LabelFrame(anzeigeoptionen_oben, text="Ansicht")
ansicht_frame.pack(fill="x", pady=(0, 5))
Radiobutton(ansicht_frame, text="Einzeln", variable=ansicht_var,
            value="einzeln", command=ansicht_umschalten).pack(anchor="w")
Radiobutton(ansicht_frame, text="Gruppe", variable=ansicht_var,
            value="gruppe", command=ansicht_umschalten).pack(anchor="w")

# Intervall
intervall_frame = LabelFrame(anzeigeoptionen_oben, text="Anzeige-Intervall (ms)")
intervall_frame.pack(fill="x", pady=(0, 5))
interval_combo = ttk.Combobox(intervall_frame, values=[50, 100, 200, 300, 400, 600, 1000],
                              textvariable=intervall_var, state="readonly")
interval_combo.bind("<<ComboboxSelected>>", intervall_aendern)
interval_combo.pack(fill="x", padx=5, pady=5)

# Traces aktivieren
skalierung_var.trace_add("write", lambda *args: plot_starten())
max_points_var.trace_add("write", lambda *args: plot_starten())

# -------- Checkboxen (rechts unten) --------
checkbox_frame = LabelFrame(rechte_seite_frame, text="Werte anzeigen")
checkbox_frame.pack(side=TOP, fill=Y, pady=(10, 0))

for i, name in enumerate(bezeichnungen):
    cb = Checkbutton(checkbox_frame, text=name, variable=sichtbar_vars[i],
                     command=lambda i=i: checkbox_toggle(i))
    cb.pack(anchor='w')

# -------- Menübereich links --------

# COM-Port Auswahl
verbindung_frame = LabelFrame(menu_frame, text="Verbindung", padx=5, pady=5)
verbindung_frame.pack(fill="x", pady=(0, 10))

Label(verbindung_frame, text="COM-Port wählen:").pack(anchor=W)

port_control_frame = Frame(verbindung_frame)
port_control_frame.pack(fill=X, pady=5)

port_list = lade_com_ports()
anzeige_geraet_liste = [(format_entry(dev, desc), dev) for dev, desc in port_list]
optionen = [anzeige for anzeige, _ in anzeige_geraet_liste]

if optionen:
    # Versuche Arduino zu finden
    arduino_index = next((i for i, (anzeige, dev) in enumerate(anzeige_geraet_liste) if "Arduino" in anzeige), None)
    if arduino_index is not None:
        default_option = anzeige_geraet_liste[arduino_index][0]
    else:
        default_option = optionen[0]
else:
    default_option = "Kein Port gefunden"
    optionen = [default_option]  # Damit OptionMenu nicht leer ist

com_port_var.set(default_option)
com_port_optionmenu = OptionMenu(port_control_frame, com_port_var, *optionen)
com_port_optionmenu.pack(side=LEFT, expand=True, fill=X)
button_refresh = Button(port_control_frame, text="🔄 Refresh", command=refresh_com_ports)
button_refresh.pack(side=LEFT, padx=5)
button_verbinden = Button(verbindung_frame, text="🔌 Verbinden", command=verbinden)
button_verbinden.pack(fill = X, pady=5)


# Livedaten
steuerung_frame = LabelFrame(menu_frame, text="Livedaten", padx=5, pady=5)
steuerung_frame.pack(fill="x", pady=(0, 10))
button_start = Button(steuerung_frame, text="▶️ Start", command=start_datenempfang, state="disabled")
button_start.pack(side=LEFT, expand=True, fill=X)
button_stop = Button(steuerung_frame, text="⏹ Stop", command=stop_datenempfang, state="disabled")
button_stop.pack(side=LEFT, expand=True, fill=X, padx=5)

# Datei / Ausgabe
datei_frame = LabelFrame(menu_frame, text="Datei & Ausgabe", padx=5, pady=5)
datei_frame.pack(fill="x", pady=(0, 10))
Button(datei_frame, text="📁 Ausgabeordner öffnen", command=ordner_öffnen, width=20).pack(pady=5)
csv_dateiname_label = Label(datei_frame, text="Keine Datei aktiv", fg="blue", wraplength=200, justify=LEFT)
csv_dateiname_label.pack()

# CSV-Block
csv_block_frame = LabelFrame(menu_frame, text="CSV-Daten", padx=5, pady=5)
csv_block_frame.pack(fill="x", pady=(10, 0))
button_csv_laden = Button(csv_block_frame, text="📁 CSV laden", command=csv_laden, width=20)
button_csv_laden.pack(pady=(0, 5))

csv_slider_label = Label(csv_block_frame, text="Scrollen durch CSV")
csv_slider_label.pack_forget()
csv_slider = Scale(csv_block_frame, from_=0, to=100, orient=HORIZONTAL, length=200, command=csv_slider_update)
csv_slider.pack_forget()

# Übersicht aktuelle Werte
werte_uebersicht_frame = LabelFrame(menu_frame, text="Werte Übersicht")
werte_uebersicht_frame.pack(fill=X, pady=(10,0))

for i in range(10):
    lbl = Label(werte_uebersicht_frame, text=f"{bezeichnungen[i]}: -")
    lbl.pack(anchor="w")
    werte_labels.append(lbl)

platzhalter = Label(menu_frame, text="", height=2)
platzhalter.pack(fill='x')
info_label = Label(menu_frame, text="", fg="red", wraplength=220, justify="left", anchor="w")
info_label.pack()

# Entwicklerinfo
ueber_button = Button(menu_frame, text="Info", command=zeige_ueber_info)
ueber_button.place(relx=0.0, rely=1.0, anchor="sw", x=10, y=-10)

    
# ----

fenster.protocol("WM_DELETE_WINDOW", fenster.destroy)
fenster.after(100, leeren_plot_initialisieren)
fenster.mainloop()