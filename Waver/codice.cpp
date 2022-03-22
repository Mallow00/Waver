#include <cstdio>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include "dependencies/include/libpq-fe.h"

using namespace std;

#define PG_HOST		"192.168.0.108"
#define PG_USER		"postuser"
#define PG_DB		"hotelnotify"
#define PG_PASS		"postpassword"
#define PG_PORT		5432

void printData(PGresult* , int , int);
void checkResults(PGresult*, const PGconn*);
void eseguiQueryUno(PGconn*);
void eseguiQueryDue(PGconn*);
void eseguiQueryTre(PGconn*);
void eseguiQueryQuattro(PGconn*);
void eseguiQueryCinque(PGconn*);
void eseguiQuerySei(PGconn*);
std::string to_str(int);

int main(int argc, char** argv) {
    char ConnectionInfo[250];
    sprintf(ConnectionInfo,"user =%s password =%s dbname =%s hostaddr =%s port =%d",PG_USER , PG_PASS , PG_DB , PG_HOST , PG_PORT);
    PGconn* conn = PQconnectdb(ConnectionInfo);
    //Verifica Conessione
    if (PQstatus(conn) != CONNECTION_OK) {
		cout << "Errore di Connessione: " << PQerrorMessage(conn);
		exit(1);
	}
    cout << "Connessione Stabilita" << endl;

    //Scelta sulle operazioni da fare
    int scelta=1;  
    while(scelta>=1 && scelta <=6){
        cout << "_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-" <<endl;
        cout << "1) Mostrare i clienti con almeno una prenotazione con acconto o senza pagamento e per ciascuno il saldo totale da pagare di tutte le prenotazioni ancora da saldare." <<endl;
        cout << "2) Mostrare il funzionamento degli alert."<<endl;
        cout << "3) Mostrare il numero di utenti che hanno effettuato una prenotazione nel mese di Aprile a Madrid."<<endl;
        cout << "4) Mostrare tutti gli Hotel il cui indirizzo coincide con l'indirizzo della Catena di appartenenza."<<endl;
        cout << "5) Mostrare le camere disponibili in un determinato periodo di un determinato Hotel."<<endl;
        cout << "6) Mostrare la media delle prenotazioni per cliente, solo se superiori a € 400."<<endl;
        cout << "0) ESCI" <<endl;
        cout << "_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-_-" <<endl;

        cout << endl << "Digitare il numero desiderato..." <<endl;
        cin >> scelta; 
        switch (scelta){
            case 1:
            eseguiQueryUno(conn);
            break;
            case 2:
            eseguiQueryDue(conn);
            break;
            case 3:
            eseguiQueryTre(conn);
            break;
            case 4:
            eseguiQueryQuattro(conn);
            break;
            case 5:
            eseguiQueryCinque(conn);
            break;
            case 6:
            eseguiQuerySei(conn);
            break;
        }
    }
    
    //Disconnessione
    PQfinish(conn);
}

/**
 * Funzione printData con 3 parametri, results che contiene i risultati, colonne e righe che contengono il numero di colonne e di righe
 */
void printData(PGresult* results, int colonne, int righe) {
	for (int i = 0; i < colonne; ++i) {
		cout << PQfname(results, i);
		if(i+1 != colonne) cout << "\t\t\t";
	}
	
    cout << endl << "--------------------------------------------------------------------" << endl;
	cout << endl;

	for (int i = 0; i < righe; ++i) {
		for (int j = 0; j < colonne; j++) {
            cout << left << setw(20) << setfill(' ') << PQgetvalue(results, i, j) << "\t\t";
		}
		cout << endl;
	}
    cout << endl << "--------------------------------------------------------------------" << endl;
	cout << "FINE OUTPUT" << endl << endl;
}

/**
 * Funzione checkResults che verifica che i risultati non presentino errori o inconsistenze
 */
void checkResults(PGresult* res, const PGconn* conn) {
	if (PQresultStatus(res) != PGRES_TUPLES_OK) {
		cout << "Risultati Inconsistenti: " << PQerrorMessage(conn) << endl;
		PQclear(res);
		exit(1);
	}
}

/**
 * Funzione Query 1
 */
void eseguiQueryUno(PGconn* conn){
    PGresult* res;
    cout << "Query 1:" << endl;
    res = PQexec(conn, "DROP VIEW IF EXISTS Saldo_Prenotazione; CREATE VIEW Saldo_Prenotazione(prenotazione,saldo) AS SELECT prenotazione,(SELECT SUM(prezzototalecamera) FROM Camera_Prenotata CP WHERE CP.prenotazione = PCA.prenotazione GROUP BY CP.prenotazione)-acconto FROM Prenotazione_Con_Acconto PCA UNION SELECT CP.prenotazione, SUM(prezzototalecamera) FROM Camera_Prenotata CP,Prenotazione_Senza_Pagamento PSA WHERE CP.prenotazione = PSA.prenotazione GROUP BY CP.prenotazione; SELECT cliente, SUM(saldo) AS saldo_totale FROM Saldo_Prenotazione JOIN Prenotazione ON prenotazione = codice GROUP BY(cliente);");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);
}

/**
 * Funzione Query 2
 */
void eseguiQueryDue(PGconn* conn){
    PGresult* res;
    cout << "Query 2:" << endl;

    //Ripristino dei cambiamenti del DB per fini della query
    res = PQexec(conn, "UPDATE disponibilita_giornaliera SET tariffa = '1' WHERE camera = '4' AND data = '2021-01-01';");
    res = PQexec(conn, "UPDATE servizi_hotel SET bar = '0' WHERE hotel = '1';");
    PQclear(res);

    cout << "L'ipotetico DB professionale di HotelNotify ha un sistema di trigger che monitora i cambiamenti soprattutto in Disponibilita_Giornaliera e, in ogni caso, ogni 5 minuti avviene un cronojob che controlla la disponibilità per ogni alert. Di seguito i controlli fatti per il primo alert ('datacreazione' = '2021-01-03 04:48:47.752285' AND 'cliente' = 'joe@gmax.com')" << endl;
    cout << endl <<"Nell’istante 1 l'utente effettua la ricerca e non trova i risultati desiderati, avviene la creazione dell’alert." << endl;
    cout << endl <<"Nell’istante 2 il cronojob effettua una ricerca degli alert, sfortunatamente non è presente nessun risultato." << endl;
    
    //QUERYALERT
    res = PQexec(conn, "SELECT * FROM( SELECT hotel, DG.camera, C.nome, AD.prezzototale AS prezzo_totale_max_alert, SUM(prezzoapersona) AS prezzo_totale_disp FROM Disponibilita_Giornaliera DG JOIN Camera C ON DG.camera = C.codice JOIN Servizi_Camera SC ON SC.camera = C.codice JOIN Tariffa T ON DG.tariffa = T.id JOIN Alert_Dettaglio AD ON AD.alertdatacreazione = '2021-01-03 04:48:47.752285' AND AD.alertcliente = 'joe@gmax.com' JOIN Alert A ON A.datacreazione = '2021-01-03 04:48:47.752285' AND A.cliente = 'joe@gmax.com' WHERE data < A.datacheckout AND data >= A.datacheckin AND DG.camera = ANY(SELECT C.codice FROM camera C JOIN Hotel H ON C.hotel = H.codice JOIN Servizi_Hotel SH ON H.codice = SH.hotel WHERE H.codice = ANY(SELECT Hotel H FROM Alert_Hotel AH WHERE AH.alertdatacreazione = AD.alertdatacreazione AND AH.alertcliente = AD.Alertcliente) AND CASE WHEN AD.ascensore is distinct from NULL THEN SH.ascensore is not distinct from AD.ascensore ELSE true END AND CASE WHEN AD.bar is distinct from NULL THEN SH.bar is not distinct from AD.bar ELSE true END AND CASE WHEN AD.parcheggio is distinct from NULL THEN SH.parcheggio is not distinct from AD.parcheggio ELSE true END) AND (nlettisingoli+(nlettidoppi*2)+ndivani)>=3 AND DG.resedisponibili-DG.prenotate>=1 AND 0 < ALL (SELECT DG.resedisponibili-DG.prenotate FROM Disponibilita_Giornaliera DG WHERE DG.camera=C.codice AND DG.data>=A.datacheckin AND DG.data<A.datacheckout) AND CASE WHEN AD.tv is distinct from NULL THEN SC.tv is not distinct from AD.tv ELSE true END AND CASE WHEN AD.frigo is distinct from NULL THEN SC.frigo is not distinct from AD.frigo ELSE true END AND CASE WHEN AD.telefono is distinct from NULL THEN SC.telefono is not distinct from AD.telefono ELSE true END AND CASE WHEN AD.ac is distinct from NULL THEN SC.ac is not distinct from AD.ac ELSE true END GROUP BY (DG.camera,C.nome, C.hotel, prezzo_totale_max_alert) ) AS Disp_Per_Camera_Filtrata WHERE prezzo_totale_disp <= prezzo_totale_max_alert;");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);

    cout << endl <<"Nell’istante 3 ipotizziamo che uno dei prezzi di Disponibilita_Giornaliera sia variato da un amministratore, sia passato da tariffa 1 (€ 65.89) a tariffa 11 (€ 9)" <<endl;
    res = PQexec(conn, "UPDATE disponibilita_giornaliera SET tariffa = '11' WHERE camera = '4' AND data = '2021-01-01';");
    PQclear(res);

    cout << endl <<"Nell'istante 4) viene rifatta la QUERYALERT, ora ci sarebbero stati dei possibili risultati, se non fosse stato per i filtri" << endl;
    //QUERYALERT
    res = PQexec(conn, "SELECT * FROM( SELECT hotel, DG.camera, C.nome, AD.prezzototale AS prezzo_totale_max_alert, SUM(prezzoapersona) AS prezzo_totale_disp FROM Disponibilita_Giornaliera DG JOIN Camera C ON DG.camera = C.codice JOIN Servizi_Camera SC ON SC.camera = C.codice JOIN Tariffa T ON DG.tariffa = T.id JOIN Alert_Dettaglio AD ON AD.alertdatacreazione = '2021-01-03 04:48:47.752285' AND AD.alertcliente = 'joe@gmax.com' JOIN Alert A ON A.datacreazione = '2021-01-03 04:48:47.752285' AND A.cliente = 'joe@gmax.com' WHERE data < A.datacheckout AND data >= A.datacheckin AND DG.camera = ANY(SELECT C.codice FROM camera C JOIN Hotel H ON C.hotel = H.codice JOIN Servizi_Hotel SH ON H.codice = SH.hotel WHERE H.codice = ANY(SELECT Hotel H FROM Alert_Hotel AH WHERE AH.alertdatacreazione = AD.alertdatacreazione AND AH.alertcliente = AD.Alertcliente) AND CASE WHEN AD.ascensore is distinct from NULL THEN SH.ascensore is not distinct from AD.ascensore ELSE true END AND CASE WHEN AD.bar is distinct from NULL THEN SH.bar is not distinct from AD.bar ELSE true END AND CASE WHEN AD.parcheggio is distinct from NULL THEN SH.parcheggio is not distinct from AD.parcheggio ELSE true END) AND (nlettisingoli+(nlettidoppi*2)+ndivani)>=3 AND DG.resedisponibili-DG.prenotate>=1 AND 0 < ALL (SELECT DG.resedisponibili-DG.prenotate FROM Disponibilita_Giornaliera DG WHERE DG.camera=C.codice AND DG.data>=A.datacheckin AND DG.data<A.datacheckout) AND CASE WHEN AD.tv is distinct from NULL THEN SC.tv is not distinct from AD.tv ELSE true END AND CASE WHEN AD.frigo is distinct from NULL THEN SC.frigo is not distinct from AD.frigo ELSE true END AND CASE WHEN AD.telefono is distinct from NULL THEN SC.telefono is not distinct from AD.telefono ELSE true END AND CASE WHEN AD.ac is distinct from NULL THEN SC.ac is not distinct from AD.ac ELSE true END GROUP BY (DG.camera,C.nome, C.hotel, prezzo_totale_max_alert) ) AS Disp_Per_Camera_Filtrata WHERE prezzo_totale_disp <= prezzo_totale_max_alert;");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);
    
    cout << endl <<"Il cliente infatti necessità di un posto con 'bar', 'wifi', 'parcheggio' ma non vuole 'tv' perchè non vuole distrazioni. Attualmente le camere disponibili al Hotel Nora Vai 1 non hanno 'bar'" << endl;
    cout << endl <<"Nell'istante 5), successivamente all'inaugurazione, l'Hotel Nora Vai rende disponibile un Bar per tutti gli ospiti dell'hotel." << endl;
    res = PQexec(conn, "UPDATE servizi_hotel SET bar = '1' WHERE hotel = '1';");
    PQclear(res);

    cout << endl <<"Nell'istante 5) se viene rifatta la QUERYALERT ora ci sono dei risultati:" << endl;
    //QUERYALERT
    res = PQexec(conn, "SELECT * FROM( SELECT hotel, DG.camera, C.nome, AD.prezzototale AS prezzo_totale_max_alert, SUM(prezzoapersona) AS prezzo_totale_disp FROM Disponibilita_Giornaliera DG JOIN Camera C ON DG.camera = C.codice JOIN Servizi_Camera SC ON SC.camera = C.codice JOIN Tariffa T ON DG.tariffa = T.id JOIN Alert_Dettaglio AD ON AD.alertdatacreazione = '2021-01-03 04:48:47.752285' AND AD.alertcliente = 'joe@gmax.com' JOIN Alert A ON A.datacreazione = '2021-01-03 04:48:47.752285' AND A.cliente = 'joe@gmax.com' WHERE data < A.datacheckout AND data >= A.datacheckin AND DG.camera = ANY(SELECT C.codice FROM camera C JOIN Hotel H ON C.hotel = H.codice JOIN Servizi_Hotel SH ON H.codice = SH.hotel WHERE H.codice = ANY(SELECT Hotel H FROM Alert_Hotel AH WHERE AH.alertdatacreazione = AD.alertdatacreazione AND AH.alertcliente = AD.Alertcliente) AND CASE WHEN AD.ascensore is distinct from NULL THEN SH.ascensore is not distinct from AD.ascensore ELSE true END AND CASE WHEN AD.bar is distinct from NULL THEN SH.bar is not distinct from AD.bar ELSE true END AND CASE WHEN AD.parcheggio is distinct from NULL THEN SH.parcheggio is not distinct from AD.parcheggio ELSE true END) AND (nlettisingoli+(nlettidoppi*2)+ndivani)>=3 AND DG.resedisponibili-DG.prenotate>=1 AND 0 < ALL (SELECT DG.resedisponibili-DG.prenotate FROM Disponibilita_Giornaliera DG WHERE DG.camera=C.codice AND DG.data>=A.datacheckin AND DG.data<A.datacheckout) AND CASE WHEN AD.tv is distinct from NULL THEN SC.tv is not distinct from AD.tv ELSE true END AND CASE WHEN AD.frigo is distinct from NULL THEN SC.frigo is not distinct from AD.frigo ELSE true END AND CASE WHEN AD.telefono is distinct from NULL THEN SC.telefono is not distinct from AD.telefono ELSE true END AND CASE WHEN AD.ac is distinct from NULL THEN SC.ac is not distinct from AD.ac ELSE true END GROUP BY (DG.camera,C.nome, C.hotel, prezzo_totale_max_alert) ) AS Disp_Per_Camera_Filtrata WHERE prezzo_totale_disp <= prezzo_totale_max_alert;");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);

    cout << endl <<"L'alert, dopo aver monitorato il DB, ora rispecchia i parametri di ricerca del utente, e può fornire un risultato, viene contrassegnato l'alert come effettuato" << endl;
    res = PQexec(conn, "UPDATE alert SET effettuato = '1' WHERE datacreazione = '2021-01-03 04:48:47.752285' AND cliente = 'joe@gmax.com';");
    PQclear(res);
}

/**
 * Funzione Query 3
 */
void eseguiQueryTre(PGconn* conn){
    PGresult* res;
    cout << "Query 3:" << endl;
    res = PQexec(conn, "SELECT COUNT(DISTINCT cliente) AS numero_clienti FROM Camera_Prenotata CP JOIN Camera C ON(CP.camera = C.codice) JOIN Hotel H ON (C.hotel = H.codice) JOIN Indirizzo I ON (H.indirizzo = I.id) JOIN Prenotazione P ON (CP.prenotazione = P.codice) WHERE I.citta = 'Madrid' AND EXTRACT (MONTH FROM P.datacheckin) = 4 OR EXTRACT (MONTH FROM P.datacheckout) = 4;");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);
}

/**
 * Funzione Query 4
 */
void eseguiQueryQuattro(PGconn* conn){
    PGresult* res;
    cout << "Query 4:" << endl;
    res = PQexec(conn, "SELECT C.nome AS nome_catena,H.nome AS nome_hotel,I.via AS via FROM Catena C,Indirizzo I,Hotel H WHERE C.indirizzo=I.id AND I.id=H.indirizzo AND C.indirizzo=H.indirizzo;");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);
}

/**
 * Funzione Query 5
 */
void eseguiQueryCinque(PGconn* conn){

    PGresult* res;
    cout << "Query 5:" << endl;
    cout << "Di seguito è necessario digitare il periodo desiderato." <<endl;
    string checkin, checkout;
    cout << "Digitare la data del CheckIn con il formato YYYY/MM/DD:" <<endl;
    cin >> checkin;
    cout << "Digitare la data del CheckOut con il formato YYYY/MM/DD:" <<endl;
    cin >> checkout;
    cout << "Le date inserite sono " << checkin << " per il checkIn e " << checkout << " per il CheckOut." << endl << endl;
    cout << "Di seguito la lista degli Hotel presenti all'interno del Database: "<<endl;

    int scelta = 1;
    while(scelta==1){

        res = PQexec(conn, "SELECT codice AS Codice, nome AS Nome FROM Hotel;");
        checkResults(res, conn);
        printData(res, PQnfields(res), PQntuples(res));
        PQclear(res);

        cout << "Digitare il codice del Hotel desiderato: " << endl;
        int codice = 0;
        cin >> codice;
        cout << "Di seguito verranno mostrate le camere disponibili nel periodo selezionato per l'Hotel selezionato: " << endl;
        std::string query = "SELECT C.nome, MIN(DG.resedisponibili-DG.prenotate) AS numero_camere_disponibili FROM Hotel H, Camera C, Disponibilita_Giornaliera DG WHERE H.codice=C.hotel AND C.codice=DG.camera AND H.codice='"+to_str(codice)+"' AND DG.data>='"+checkin+"' AND DG.data<'"+checkout+"' AND 0 < ALL(SELECT DG.resedisponibili-DG.prenotate FROM Disponibilita_Giornaliera DG WHERE DG.camera=C.codice AND DG.data>='"+checkin+"' AND DG.data<'"+checkout+"') GROUP BY(C.nome);";
        res = PQexec(conn, query.c_str());
        checkResults(res, conn);
        printData(res, PQnfields(res), PQntuples(res));
        PQclear(res);

        cout << "Se si desidera visualizzare un'altro hotel per lo stesso periodo digitare 1, altrimenti 0." << endl;
        cin >> scelta;
    }
}


/**
 * Funzione Query 6
 */
void eseguiQuerySei(PGconn* conn){
    PGresult* res;
    cout << "Query 6:" << endl;
    res = PQexec(conn, "DROP VIEW IF EXISTS Totale_Prenotazione; CREATE VIEW Totale_Prenotazione(prenotazione,totale) AS SELECT P.codice,SUM(CP.prezzototalecamera) FROM Prenotazione P,Camera_Prenotata CP WHERE P.codice=CP.prenotazione GROUP BY(P.codice); SELECT C.nominativo, ROUND(AVG(TP.totale),2) AS media_prenotazioni_effettuate FROM Cliente C,Prenotazione P,Totale_Prenotazione TP WHERE C.utente=P.cliente AND P.codice=TP.prenotazione GROUP BY(C.nominativo) HAVING AVG(TP.totale)>400 ORDER BY(AVG(TP.totale)) DESC;");
    checkResults(res, conn);
    printData(res, PQnfields(res), PQntuples(res));
    PQclear(res);
}

std::string to_str(int i)
{
    std::stringstream ss;
    ss << i;
    return ss.str();
}
