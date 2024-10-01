#include <stdio.h>
#include "pico/stdlib.h"
#include "bmp280.h"
#include <math.h>
#include <string.h>

// Constantes para el cálculo de altura
double l = 0.0065; // [K/m] Taza de decremento de temperatura en la atmósfera terrestre
double p_0 = 101325; // [Pa] Presión constante a nivel del mar
double r = 8.314; // [j / mol.k] Constante de gas ideal
double g = 9.80665; // [m/s^2] Gravedad
double m = 0.029; // [kg/mol] Masa molar del aire

int main(void) {
    stdio_init_all();
    sleep_ms(5000);

    // Configuración del i2c
    i2c_init(i2c0, 400000);
    gpio_set_function(4, GPIO_FUNC_I2C);
    gpio_set_function(5, GPIO_FUNC_I2C);
    gpio_pull_up(4);
    gpio_pull_up(5);

    // Inicio bmp280
    bmp280_init(i2c0);
    struct bmp280_calib_param params;
    bmp280_get_calib_params(&params);

    printf("before char \n");
    // Creo archivo Memo, modo append, para crearlo nomás
    const char *memo = "memo.txt";

    while(1) {
        // Leo valores de presión y temperatura con el bmp280
        int32_t temp_raw, pres_raw;
        bmp280_read_raw(&temp_raw, &pres_raw);
        int32_t temp = bmp280_convert_temp(temp_raw, &params);
        int32_t pres = bmp280_convert_pressure(pres_raw, temp_raw, &params); // En pascales

        // Declaro variables de temperatura en °C y °K
        double temp_c = temp / 100.0;
        double temp_k = temp_c + 273.15;
        // Cálculo de altura en función de la temperatura y la presión
        double height = ((temp_k / l) * (1 - (pow((p_0 / pres), ((r * l) / (g * m)))))) / 5;

        // Imprimo valores en consola
        printf("Temperatura Celcius %f C\n", temp_c);
        printf("Temperatura Kelvin %f C\n", temp_k);
        printf("Presion %ld Pa\n", pres);
        printf("Altura: %f m\n\n", height);

        FILE *archivo = fopen(memo, "a+");
        // Copio valores dentro del archivo Memo
        fprintf(archivo, "%f°C\n%ldPa\n %fm\n\n", temp_c, pres, height);

        char buffer[500];

        fclose(archivo);
        sleep_ms(5000);
    }
}