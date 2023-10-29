#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

struct Parqueadero {
    string codigo;
    bool ocupado;
    string placa;
    time_t horaIngreso;
};

class Edificio {
public:
    Edificio(int pisos, int espaciosPorPiso) : pisos(pisos), espaciosPorPiso(espaciosPorPiso) {
        for (int piso = 1; piso <= pisos; ++piso) {
            for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                Parqueadero parqueadero = {"P" + to_string(piso) + "-" + to_string(espacio), false, "******", 0};
                parqueaderos.push_back(parqueadero);
            }
        }
    }

    void abrirParqueadero(time_t horaApertura) {
        this->horaApertura = horaApertura;
        ifstream reporte("reporte.txt");
        
        if (reporte.is_open()) {
            for (int piso = 1; piso <= pisos; ++piso) {
                for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                    Parqueadero& parqueadero = parqueaderos[(piso - 1) * espaciosPorPiso + (espacio - 1)];
                    int ocupado;
                    reporte >> parqueadero.codigo >> ocupado >> parqueadero.placa;
                    parqueadero.ocupado = ocupado == 1;
                    if (parqueadero.ocupado) {
                        parqueadero.horaIngreso = horaApertura;
                    }
                }
            }

            reporte.close();
        }
    }

    void registrarNovedad(time_t horaActual, string placa) {
        if (horaActual < horaApertura || (horaCierre != 0 && horaActual > horaCierre)) {
            cout << "La hora actual esta fuera del horario de apertura del parqueadero.\n";
            return;
        }

        int indice = rand() % (pisos * espaciosPorPiso);
        Parqueadero& parqueadero = parqueaderos[indice];
        
        if (parqueadero.ocupado) {
            double minutosParqueo = difftime(horaActual, parqueadero.horaIngreso) / 60;
            double valorAPagar = calcularPago(minutosParqueo);
            cout << "Vehiculo " << parqueadero.placa << " sale del parqueadero " << parqueadero.codigo << ". Valor a pagar: " << valorAPagar << "\n";
            parqueadero.ocupado = false;
            parqueadero.placa = "******";
            parqueadero.horaIngreso = 0;
        } else {
            parqueadero.ocupado = true;
            parqueadero.placa = placa;
            parqueadero.horaIngreso = horaActual;
            cout << "Vehiculo " << placa << " asignado al parqueadero " << parqueadero.codigo << "\n";
        }
    }

    void cerrarParqueadero(time_t horaCierre) {
        this->horaCierre = horaCierre;
        ofstream reporte("reporte.txt");
        
        for (int piso = 1; piso <= pisos; ++piso) {
            for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                Parqueadero& parqueadero = parqueaderos[(piso - 1) * espaciosPorPiso + (espacio - 1)];
                reporte << parqueadero.codigo << " " << (parqueadero.ocupado ? 1 : 0) << " " << parqueadero.placa << " ";
                if (parqueadero.ocupado) {
                    double minutosParqueo = difftime(horaCierre, parqueadero.horaIngreso) / 60;
                    double valorAPagar = calcularPago(minutosParqueo);
                    cout << "Vehiculo " << parqueadero.placa << " debe pagar: " << valorAPagar << "\n";
                }
            }
            reporte << "\n";
        }

        reporte.close();
    }

private:
    int pisos;
    int espaciosPorPiso;
    vector<Parqueadero> parqueaderos;
    time_t horaApertura;
    time_t horaCierre = 0;

    double calcularPago(double minutosParqueo) {
        return max(15.0, minutosParqueo) * 150;
    }
};

int main() {
    Edificio edificio(6, 20);
    
    int dia, mes, ano, hora, minuto, segundo;
    cout << "Ingrese la fecha y hora de apertura del parqueadero (dia mes ano hora minuto segundo): ";
    cin >> dia >> mes >> ano >> hora >> minuto >> segundo;

    tm tiempoApertura = {0};
    tiempoApertura.tm_year = ano - 1900;
    tiempoApertura.tm_mon = mes - 1;
    tiempoApertura.tm_mday = dia;
    tiempoApertura.tm_hour = hora;
    tiempoApertura.tm_min = minuto;
    tiempoApertura.tm_sec = segundo;
    time_t horaApertura = mktime(&tiempoApertura);

    edificio.abrirParqueadero(horaApertura);

    cout << "Ingrese la fecha y hora de cierre del parqueadero (dia mes ano hora minuto segundo): ";
    cin >> dia >> mes >> ano >> hora >> minuto >> segundo;

    tm tiempoCierre = {0};
    tiempoCierre.tm_year = ano - 1900;
    tiempoCierre.tm_mon = mes - 1;
    tiempoCierre.tm_mday = dia;
    tiempoCierre.tm_hour = hora;
    tiempoCierre.tm_min = minuto;
    tiempoCierre.tm_sec = segundo;
    time_t horaCierre = mktime(&tiempoCierre);

    edificio.cerrarParqueadero(horaCierre);

    int numNovedades;
    cout << "Ingrese el numero de novedades que desea registrar: ";
    cin >> numNovedades;
    
    for (int i = 0; i < numNovedades; ++i) {
        int hora, minuto, segundo;
        string placa;
        cout << "Ingrese la placa del vehiculo: ";
        cin >> placa;
        cout << "Ingrese la hora de la novedad (hora minuto segundo): ";
        cin >> hora >> minuto >> segundo;

        tm tiempoNovedad = {0};
        tiempoNovedad.tm_year = ano - 1900;
        tiempoNovedad.tm_mon = mes - 1;
        tiempoNovedad.tm_mday = dia;
        tiempoNovedad.tm_hour = hora;
        tiempoNovedad.tm_min = minuto;
        tiempoNovedad.tm_sec = segundo;
        time_t horaActual = mktime(&tiempoNovedad);

        edificio.registrarNovedad(horaActual, placa);
    }

    return 0;
}