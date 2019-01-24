#include <iostream>
#include <string>
#include <sstream>
#include <bitset>
#include <iostream>
#include <chrono>
typedef std::chrono::high_resolution_clock Clock;
using namespace std;
//array um das sudoku zu speichern
int s[81];
/*array um die zahlen in ein anderes system umzurechnen, d.h.:
 * Dezimal -> binär
* 1 wird zu 000000001
* 2 wird zu 000000010
* 3 wird zu 000000100
* 4 wird zu 000001000
* 5 wird zu 000010000
* 6 wird zu 000100000
* 7 wird zu 001000000
* 8 wird zu 010000000
* 9 wird zu 100000000
*/
int n[] = {0, 1, 2, 4, 8, 16, 32, 64, 128, 256};
//default zahl ist 511, da das genau 111111111
const int reset = 511;
// shifts für zellenüberprüfung (schneller als 2 for schleifen um die indices durchzugehen)
int o[] = {0, 1, 2, 9, 10, 11, 18, 19, 20};
// direkter verweis auf die erste zahl der zelle, damit es aus der position des backtracking mit einer opperation errechnet wird
//vielleicht egal :D
int c[81];


//Methode zum checken auf einschränkungen der zeile
void checkLine(int pos)
{
    //start und stop der reihe
    int start = pos - (pos % 9);
    int stop = start + 9;
    //loop über die reihe
    for (int i = start; i < stop; i++)
    {
        //checkt ob eine zahl oder eine reset value dasteht, und check damit die zahl sich nicht selbst auf 0 schreibt.
        if (s[i] <= 256 && i != pos)
        {
            /* kernlogik: 
            * der wert von s[pos] = 1'1111'1111 in binär,
            * wenn jetzt z.b. eine 8 irgendwo gefunden wird setzt diese zeile s[pos] auf 1'0111'1111, da die 8 als 0'1000'0000
            * gespeichert ist
            * ca 4 mal schneller als naive normale vergleiche mit zahlen, da es 9 möglichkeiten zu probieren gibt, und die zeile nur
            * 2 cpu zyclen braucht.
            */
            s[pos] ^= s[pos] & s[i];
        }
    }
}

//Methode zum checken auf Einschränkungen der Reihe (gleich wie für Zeile)
void checkCol(int pos)
{
    int start = pos % 9;
    for (int i = start; i < 81; i += 9)
    {
        if (s[i] <= 256 && i != pos)
        {
            s[pos] ^= s[pos] & s[i];
        }
    }
}

//Methode zum checken auf Einschränkungen der Zelle (gleich wie für Zeile)
void checkCell(int pos)
{
    int tPos = c[pos];
    for (int i = 0; i < 9; i++)
    {
        if (s[tPos + o[i]] <= 256 && (tPos + o[i]) != pos)
        {
            s[pos] ^= s[tPos + o[i]] & s[pos];
        }
    }
}

//sammelmethode um alle checks auf einer stelle zu haben (nur für die übersicht :D)
int checkAvailable(int pos)
{
    checkLine(pos);
    checkCol(pos);
    checkCell(pos);
    //gibt das ergebnis des checks zurück
    return s[pos];
}

//backtrackinglogik, vemutlich noch nicht optimal (rekursive schreibweise -> erzeugt grossen memory overhead -> kann verlangsamen)
int backtrack(int pos)
{
    //prüft, ob das tracking bis erfolgreich zum ende gekommen ist (wenn 1 zurückgegeben wird ist ein ergebnis gefunden)
    if(pos == 81) {
        return 1;
    }
    //prüft ob die stelle eine vorgeschriebene zahl ist, wenn ja überspringen
    else if (s[pos] != 511)
    {
        return backtrack(pos + 1);
    }
    //hauptarbeitszweig
    else{
        // checkt, welche Zahlen möglich wären
        int possibles = checkAvailable(pos);
        // geht in der Suche zurück wenn keine Lösung mehr möglich ist mit den getesteten Zahlen.
        if (possibles == 0)
        {
            // resets current position
            s[pos] = reset;
            // 0 für weiteres backtracking
            return 0;
        }
        //geht über alle Zahlen von 1 bis 9
        for (int i = 1; i < 10; i++)
        {
            //Prüfung ob die Zahl i möglich wäre
            if ((possibles & n[i]))
            {
                //Schreibe Zahl in binär (n[i]) an die position
                s[pos] = n[i];
                //gehe einen Schritt weiter, wenn 1 zurückkommt Erfolgreiche lösung daher kann dann abgebrochen werden
                if(backtrack(pos + 1))
                    return 1;
            }
        }
    }
    //default: keine Lösung, also gehe zurück und resette die aktuelle Position
    s[pos] = reset;
    return 0;
}

// hilfs Methode zum rückrechnen der Zahlen in normale repräsentation (geht nicht besonderst schnell oder sinnvoll)
char getNumber(int i)
{
    char ret;
    switch (i)
    {
    case 0:
        ret = 0;
        break;
    case 1:
        ret = 1;
        break;
    case 2:
        ret = 2;
        break;
    case 4:
        ret = 3;
        break;
    case 8:
        ret = 4;
        break;
    case 16:
        ret = 5;
        break;
    case 32:
        ret = 6;
        break;
    case 64:
        ret = 7;
        break;
    case 128:
        ret = 8;
        break;
    case 256:
        ret = 9;
        break;

    default:
        ret = '/' - 48;
        break;
    }
    return ret + 48;
}

//schreibt Sudoku auf die Konsole
void writeSudoku()
{
    for (int i = 0; i < 81; i++)
    {
        if (i % 3 == 0 && i != 0)
            cout << " |";
        if (i % 9 == 0 && i != 0)
            cout << "\n";
        if (i % 27 == 0 && i != 0)
            cout << "-------+-------+-------+\n";
        cout << " " << getNumber(s[i]);
    }
    cout << endl;
}

int main()
{
    string mystr;
    cout << "What's your Sudoku? ";
    //einlesen des sudokus als string der Form ZeileZeileZeile -> 0020210105020870201... (ohne iwelche umbrüche, 0 für lehre felder)
    getline(cin, mystr);
    //starte Timer
    auto t1 = Clock::now();
    //Schleife über Eingabe
    for (int i = 0; mystr[i] != '\0'; i++)
    {
        //konvertieren von zeichen zu int und dann konvertierung in binäre representation
        s[i] = n[(int)(mystr[i] - 48)];
        //etzen der lehren felder auf die reset value
        if (s[i] == 0)
            s[i] = reset;
        //Vorberechnung der Offsets für die Zellen (hier logisch etwas schwachsinnig, aber schleife von 0 bis 80, daher möglich)
        int temp = i - (i % 3);
        temp = temp - ((temp / 9) % 3) * 9;
        c[i] = temp;
    }
    //backtracking
    backtrack(0);
    //stoppe Timer
    auto t2 = Clock::now();
    //komplizierte Timerausgabe
    std::cout << "Delta t2-t1: " 
              << std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count()
              << " nano" << std::endl;
    //schreibt Sudoku auf Konsole
    writeSudoku();

    return 0;
}