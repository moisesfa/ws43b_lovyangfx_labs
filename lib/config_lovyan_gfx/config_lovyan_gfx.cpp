#include <LovyanGFX.hpp>
#include "config_lovyan_gfx.h"
#include "tab_01.h"
#include "tab_02.h"
#include "tabs_all.h"

// Creamos el objeto
LGFX tft;

// Valores configurables para los umbrales de detección
#define GESTURE_THRESHOLD_X 40 // Desplazamiento mínimo en X para un gesto
#define GESTURE_THRESHOLD_Y 40 // Desplazamiento mínimo en Y para un gesto

// Cola para comunicar gestos
QueueHandle_t gestureQueue;

// Enumeración para almacenar gestos
enum Gesture
{
    NONE,
    UP,
    DOWN,
    LEFT,
    RIGHT
};
// Estructura para almacenar coordenadas
struct TouchData
{
    int16_t x;
    int16_t y;
    bool touched;
};

// Tarea para leer coordenadas táctiles y detectar gestos
void taskReadTouchAndDetectGesture(void *pvParameters)
{
    lgfx::touch_point_t touch;
    TouchData touchData;
    int16_t prevX = -1, prevY = -1;
    Gesture detectedGesture = NONE;

    while (true)
    {
        touchData.touched = tft.getTouch(&touch); // Verifica si hay toque
        if (touchData.touched)
        {
            touchData.x = touch.x;
            touchData.y = touch.y;
            //Serial.printf("X:%d Y:%d\n", touch.x, touch.y);

            // Detectar gestos si hay una posición previa válida
            if (prevX != -1 && prevY != -1)
            {
                int16_t deltaX = touchData.x - prevX;
                int16_t deltaY = touchData.y - prevY;

                if (abs(deltaX) > abs(deltaY))
                { // Predomina el movimiento horizontal
                    if (deltaX > GESTURE_THRESHOLD_X)
                    {
                        detectedGesture = RIGHT;
                    }
                    else if (deltaX < -GESTURE_THRESHOLD_X)
                    {
                        detectedGesture = LEFT;
                    }
                }
                else
                { // Predomina el movimiento vertical
                    if (deltaY > GESTURE_THRESHOLD_Y)
                    {
                        detectedGesture = DOWN;
                    }
                    else if (deltaY < -GESTURE_THRESHOLD_Y)
                    {
                        detectedGesture = UP;
                    }
                }
            }

            // Guardar las coordenadas actuales como referencia
            prevX = touchData.x;
            prevY = touchData.y;
        }
        else
        {
            // Si no hay toque, reiniciar coordenadas previas
            prevX = -1;
            prevY = -1;
        }

        // Enviar gesto detectado a la cola
        xQueueSend(gestureQueue, &detectedGesture, portMAX_DELAY);

        // Resetear gesto después de enviar
        detectedGesture = NONE;

        vTaskDelay(pdMS_TO_TICKS(50)); // Leer cada 50 ms
    }
}

// Tarea para gestionar los gestos
void taskManagementGesture(void *pvParameters)
{
    Gesture gesture;

    while (true)
    {
        // Recibir datos de la cola
        if (xQueueReceive(gestureQueue, &gesture, portMAX_DELAY))
        {
            switch (gesture)
            {
            case UP:
                Serial.println("Gesture: UP");
                break;
            case DOWN:
                Serial.println("Gesture: DOWN");
                break;
            case LEFT:
                if (tab_number != 2)
                    tab_02_view();
                Serial.println("Gesture: LEFT");
                break;
            case RIGHT:
                if (tab_number != 1)
                    tab_01_view();
                Serial.println("Gesture: RIGHT");
                break;
            case NONE:
            default:
                break;
            }
        }
    }
}

void init_lovyangfx()
{
    Serial.println("Lgfx init...");

    if (psramFound())
    {
        Serial.println("PSRAM detectada y habilitada");
        Serial.println("Memoria libre en PSRAM: " + String(ESP.getFreePsram()) + " bytes");
        Serial.println("Memoria libre en SRAM: " + String(ESP.getFreeHeap()) + " bytes");
    }
    else
    {
        Serial.println("PSRAM no detectada");
        Serial.println("Memoria libre en SRAM: " + String(ESP.getFreeHeap()) + " bytes");
    }

    tft.init();

    //  tft.setRotation(0);
    tab_01_view();

    // Crear cola para los gestos
    gestureQueue = xQueueCreate(5, sizeof(Gesture));
    if (gestureQueue == NULL)
    {
        Serial.println("Error al crear cola");
        while (true)
            ; // Detener en caso de error
    }

    // Crear tareas FreeRTOS
    xTaskCreate(taskReadTouchAndDetectGesture, "TaskReadTouchAndDetectGesture", 1024 * 2, NULL, 1, NULL);
    xTaskCreate(taskManagementGesture, "TaskManagementGesture", 1024 * 2, NULL, 1, NULL);
}
