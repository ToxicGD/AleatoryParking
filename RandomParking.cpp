#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>

using namespace std;

// Estructura para representar un parqueadero
struct Parqueadero {
    string codigo;
    bool ocupado;
    string placa;
    time_t horaIngreso;
};

// Clase para representar el edificio
class Edificio {
public:
    // Constructor que inicializa el edificio con un número específico de pisos y espacios por piso
    Edificio(int pisos, int espaciosPorPiso) : pisos(pisos), espaciosPorPiso(espaciosPorPiso) {
        for (int piso = 1; piso <= pisos; ++piso) {
            for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                Parqueadero parqueadero = {"P" + to_string(piso) + "-" + to_string(espacio), false, "******", 0};
                parqueaderos.push_back(parqueadero);
            }
        }
    }

    // Función para abrir el parqueadero y cargar el estado de los parqueaderos desde un archivo
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

    // Función para registrar una novedad (entrada o salida de un vehículo)
    void registrarNovedad(time_t horaActual) {
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
            string placa;
            cout << "Ingrese la placa del vehiculo: ";
            cin >> placa;

            parqueadero.ocupado = true;
            parqueadero.placa = placa;
            parqueadero.horaIngreso = horaActual;
            cout << "Vehiculo " << placa << " asignado al parqueadero " << parqueadero.codigo << "\n";
        }

        // Actualizar el archivo de reporte después de registrar la novedad
        actualizarReporte();
    }

    // Función para cerrar el parqueadero y generar los archivos de reporte
    void cerrarParqueadero(time_t horaCierre) {
        this->horaCierre = horaCierre;

        // Generar el archivo de facturación
        ofstream facturacion("facturacion.txt");
        
        for (int piso = 1; piso <= pisos; ++piso) {
            for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                Parqueadero& parqueadero = parqueaderos[(piso - 1) * espaciosPorPiso + (espacio - 1)];
                if (parqueadero.ocupado) {
                    double minutosParqueo = difftime(horaCierre, parqueadero.horaIngreso) / 60;
                    double valorAPagar = calcularPago(minutosParqueo);
                    facturacion << parqueadero.placa << " " << parqueadero.codigo << " " << asctime(localtime(&parqueadero.horaIngreso)) << asctime(localtime(&horaCierre)) << valorAPagar << "\n";
                }
            }
        }

        facturacion.close();

        // Actualizar el archivo de reporte al final del día
        actualizarReporte();
    }

private:
    int pisos; // Número de pisos en el edificio
    int espaciosPorPiso; // Número de espacios por piso
    vector<Parqueadero> parqueaderos; // Vector para almacenar los parqueaderos
    time_t horaApertura; // Hora de apertura del parqueadero
    time_t horaCierre = 0; // Hora de cierre del parqueadero

    // Función para calcular el pago basado en los minutos de parqueo
    double calcularPago(double minutosParqueo) {
        return max(15.0, minutosParqueo) * 150;
    }

    // Función para actualizar el archivo de reporte
    void actualizarReporte() {
        ofstream reporte("reporte.txt");
        
        for (int piso = 1; piso <= pisos; ++piso) {
            for (int espacio = 1; espacio <= espaciosPorPiso; ++espacio) {
                Parqueadero& parqueadero = parqueaderos[(piso - 1) * espaciosPorPiso + (espacio - 1)];
                reporte << parqueadero.codigo << " " << (parqueadero.ocupado ? 1 : 0) << " " << parqueadero.placa;
                if (espacio != espaciosPorPiso) {
                    reporte << " ";
                }
            }
            reporte << "\n";
        }

        reporte.close();
    }
};

int main() {
    // Crear un edificio con 6 pisos y 20 espacios por piso
    Edificio edificio(6, 20);
    
    int dia, mes, ano, hora, minuto, segundo;
    cout << "Ingrese la fecha y hora de apertura del parqueadero (dia mes ano hora minuto segundo): ";
    cin >> dia >> mes >> ano >> hora >> minuto >> segundo;

    // Crear un objeto tm con la fecha y hora de apertura ingresadas por el usuario
    tm tiempoApertura = {0};
    tiempoApertura.tm_year = ano - 1900;
    tiempoApertura.tm_mon = mes - 1;
    tiempoApertura.tm_mday = dia;
    tiempoApertura.tm_hour = hora;
    tiempoApertura.tm_min = minuto;
    tiempoApertura.tm_sec = segundo;
    time_t horaApertura = mktime(&tiempoApertura);

    // Abrir el parqueadero con la hora de apertura especificada
    edificio.abrirParqueadero(horaApertura);

    cout << "Ingrese la fecha y hora de cierre del parqueadero (dia mes ano hora minuto segundo): ";
    cin >> dia >> mes >> ano >> hora >> minuto >> segundo;

    // Crear un objeto tm con la fecha y hora de cierre ingresadas por el usuario
    tm tiempoCierre = {0};
    tiempoCierre.tm_year = ano - 1900;
    tiempoCierre.tm_mon = mes - 1;
    tiempoCierre.tm_mday = dia;
    tiempoCierre.tm_hour = hora;
    tiempoCierre.tm_min = minuto;
    tiempoCierre.tm_sec = segundo;
    time_t horaCierre = mktime(&tiempoCierre);

    // Cerrar el parqueadero con la hora de cierre especificada
    edificio.cerrarParqueadero(horaCierre);

    int numNovedades;
    cout << "Ingrese el numero de novedades que desea registrar: ";
    cin >> numNovedades;
    
     // Bucle para registrar las novedades especificadas por el usuario
     for (int i = 0; i < numNovedades; ++i) {
         int hora, minuto, segundo;
         cout << "Ingrese la hora de la novedad (hora minuto segundo): ";
         cin >> hora >> minuto >> segundo;


    // Crear un objeto tm con la fecha y hora de la novedad ingresadas por el usuario
    tm tiempoNovedad = {0};
    tiempoNovedad.tm_year = ano - 1900;
    tiempoNovedad.tm_mon = mes - 1;
    tiempoNovedad.tm_mday = dia;
    tiempoNovedad.tm_hour = hora;
    tiempoNovedad.tm_min = minuto;
    tiempoNovedad.tm_sec = segundo;
    time_t horaActual = mktime(&tiempoNovedad);

    // Registrar la novedad en el edificio
    edificio.registrarNovedad(horaActual);
}

    return 0;
}