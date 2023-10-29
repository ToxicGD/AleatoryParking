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
        for (auto& parqueadero : parqueaderos) {
            if (parqueadero.ocupado) {
                parqueadero.horaIngreso = horaApertura;
            }
        }
    }

    void registrarNovedad(time_t horaActual) {
        if (horaActual < horaApertura || horaActual > horaCierre) {
            cout << "La hora de ingreso está fuera del horario de apertura del parqueadero.\n";
            return;
        }

        int indice = rand() % (pisos * espaciosPorPiso);
        Parqueadero& parqueadero = parqueaderos[indice];
        
        if (parqueadero.ocupado) {
            double minutosParqueo = difftime(horaActual, parqueadero.horaIngreso) / 60;
            double valorAPagar = calcularPago(minutosParqueo);
            cout << "Vehículo " << parqueadero.placa << " sale del parqueadero " << parqueadero.codigo << ". Valor a pagar: " << valorAPagar << "\n";
            parqueadero.ocupado = false;
            parqueadero.placa = "******";
            parqueadero.horaIngreso = 0;
        } else {
            string placa;
            cout << "Ingrese la placa del vehículo: ";
            cin >> placa;

            parqueadero.ocupado = true;
            parqueadero.placa = placa;
            parqueadero.horaIngreso = horaActual;
            cout << "Vehículo " << placa << " asignado al parqueadero " << parqueadero.codigo << "\n";
        }
    }

    void generarReporte() {
        ofstream reporte("reporte.txt");
        
        for (int piso = 1; piso <= pisos; ++piso) {
            for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                Parqueadero& parqueadero = parqueaderos[(piso - 1) * espaciosPorPiso + (espacio - 1)];
                reporte << parqueadero.codigo << " " << (parqueadero.ocupado ? 1 : 0) << " " << parqueadero.placa << " ";
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
    time_t horaCierre;

    double calcularPago(double minutosParqueo) {
        return max(15.0, minutosParqueo) * 150;
    }
};


int main() {
    Edificio edificio(6, 20);
    
    // Aquí puedes poner la fecha y hora de apertura que desees
    time_t horaApertura = time(0);
    tm* tiempoLocal = localtime(&horaApertura);
    tiempoLocal->tm_hour = 5;
    tiempoLocal->tm_min = 0;
    tiempoLocal->tm_sec = 0;
    horaApertura = mktime(tiempoLocal);

    edificio.abrirParqueadero(horaApertura);
    
    int numNovedades;
    cout << "Ingrese el número de novedades que desea registrar: ";
    cin >> numNovedades;
    
    for (int i = 0; i < numNovedades; ++i) {
        int hora, minuto, segundo;
        cout << "Ingrese la hora de la novedad (hora minuto segundo): ";
        cin >> hora >> minuto >> segundo;

        // Aquí puedes convertir la hora ingresada a un objeto time_t
        time_t horaActual = time(0);
        tiempoLocal = localtime(&horaActual);
        tiempoLocal->tm_hour = hora;
        tiempoLocal->tm_min = minuto;
        tiempoLocal->tm_sec = segundo;
        horaActual = mktime(tiempoLocal);

        edificio.registrarNovedad(horaActual);
    }

    edificio.generarReporte();

    return 0;
}
