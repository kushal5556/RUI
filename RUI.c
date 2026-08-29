#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

// ----- TODO ----
// -> Panel (structure, types/uses [?]) 
// -> Cross button state resolver (integrated array [?])

#define WIDTH 800
#define HEIGHT 600

// --- macro ---
#define da_append(array, item)\
    do{\
        if((array).size >= (array).capacity){\
            (array).capcity = (array).capacity == 0 ? 10 : (array).capacity * 2;\
            (array).items = realloc((array).items, sizeof(*(array).items)*(array).capcity);\
            if((array).items == NULL){perror("[DA_APPEND]: Failed to Realloc\n"); exit(1);}\
        }\
        (array).items[(array).size++] = item;\
    }while(0)

// --- global variables --------
const Color SILVER = {192, 192, 192, 255};
#define PRESSED_COUNTER 0.3f //seconds

// --- structs ------
typedef struct{
    Color normal;
    Color hover;
    Color active;
}DColor;

typedef enum{
    NORMAL, HOVER, PRESSED
}State;

typedef struct{
    Rectangle rec;
    DColor color;
    Color textColor;
    State state;
    char label[100];
}ActionButton;

typedef struct{
    Rectangle rec;
    Color color;
    State state;
    char label[100];
    Color textColor;

    float pressCounter;
}ActionButton_D; //Detailed Action Button

typedef struct{
    Rectangle rec;
    Color color;
}Panel; 
// Buttons position are relative to the panel size and position

typedef struct{
    Rectangle rec; 
    DColor color;
    Color textColor;
    State state; 
    bool isActive;
    char label[100];

    Panel panel;
}PanelButton;

/// ----- function declaration-----------
ActionButton getActionButton(float x,  float y,  float width, float height, char* label); //return the default button
ActionButton_D getActionButton_D(float x, float y, float width, float height, char* label); //return the default button_D
PanelButton getPanelButton(float x, float y, float width, float height, char* label, Panel panel); //return the default panel button
Panel getPanel(float x, float y, float width, float height); //return the default panel

void updateActionButton(ActionButton* button, Vector2 mouse);
void updateActionButton_D(ActionButton_D* button, Vector2 mouse);
void updatePanelButton(PanelButton* button, Vector2 mouse);

void drawActionButton_D(ActionButton_D button); 
void drawActionButton(ActionButton button);
void drawPanelButton(PanelButton button);

void drawText(Rectangle rec, char* label, int textOffsetX, int textOffsetY, Color color);
Rectangle scaleRec(Rectangle rec, int scale);

// --- collision function -----------
bool point_rect_collision(Rectangle rec, Vector2 point);
bool circle_point_collision(Vector2 circle, float radius, Vector2 point);

int main(){
    // -------init----------
    InitWindow(WIDTH, HEIGHT, "UI");
    SetTargetFPS(60);

    ActionButton rec = getActionButton(400, 300, 80, 50, "Button");
    ActionButton_D arec = getActionButton_D(500, 400, 80,60, "Click here");

    Panel p = getPanel(200,50,400,150);
    PanelButton pb = getPanelButton(50, 100, 40, 30, "Open", p);

    //---------game loop ----------
    while(!WindowShouldClose()){

        Vector2 mouse = GetMousePosition();

        updateActionButton(&rec, mouse);  
        updateActionButton_D(&arec, mouse);  
        updatePanelButton(&pb, mouse);

        // --- clear and draw -----
        BeginDrawing();
        ClearBackground(SKYBLUE);

        drawActionButton(rec);
        drawActionButton_D(arec);
        drawPanelButton(pb);

        EndDrawing();
    }

    // -- close----
    CloseWindow();
    return 0;
}

// ---- function definition -------
bool point_rect_collision(Rectangle rec, Vector2 point)
{
    if(point.x >= rec.x && point.x <= rec.x + rec.width &&
       point.y >= rec.y && point.y <= rec.y + rec.height){ 
            return true;
    }
    return false;
}

bool circle_point_collision(Vector2 circle, float radius, Vector2 point)
{
    float dx = circle.x - point.x;
    float dy = circle.y - point.y;

    float dist_sq = (dx*dx) + (dy*dy);
    if(dist_sq <= radius * radius){
        return true;
    }
    return false;
}

ActionButton getActionButton(float x, float y, float width, float height, char* label)
{
    ActionButton button = {
        .rec = (Rectangle){x,y,width,height},
        .color = (DColor){
            .normal  = SILVER,
            .hover   = LIGHTGRAY,
            .active  = SILVER
        },
        .textColor = BLACK,
        .state = NORMAL,
    };
    strcpy(button.label, label);

    return button; 
}

ActionButton_D getActionButton_D(float x, float y, float width, float height, char* label)
{
    ActionButton_D button = {
        .rec = (Rectangle){x,y,width,height},
        .color = SILVER,
        .textColor = BLACK,
        .state = NORMAL,
        .pressCounter = PRESSED_COUNTER
    };
    strcpy(button.label, label);

    return button; 
}

PanelButton getPanelButton(float x, float y, float width, float height, char* label, Panel panel)
{
    PanelButton button = {
        .rec = (Rectangle){x,y,width,height},
        .color = (DColor){
            .normal = SILVER,
            .hover  = LIGHTGRAY,
            .active = GRAY
        },
        .textColor = BLACK,
        .state = NORMAL,
        .isActive = false,
        .panel = panel
    };
    strcpy(button.label, label);
    return button;
}

Panel getPanel(float x, float y, float width, float height)
{
    Panel panel = {
        .rec = (Rectangle){x,y,width,height},
        .color = SILVER
    };
    return panel;
}

void updateActionButton(ActionButton* button, Vector2 mouse)
{
    button->state = NORMAL;//reset

    if(point_rect_collision(button->rec, mouse)){
        button->state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            button->state = PRESSED;
        }
    }
}

void updateActionButton_D(ActionButton_D* button, Vector2 mouse)
{
    if(button->pressCounter > PRESSED_COUNTER) button->pressCounter = PRESSED_COUNTER;

    if(button->pressCounter != PRESSED_COUNTER || button->pressCounter < PRESSED_COUNTER){
        button->pressCounter -= GetFrameTime();
        if(button->pressCounter <= 0.0f) button->pressCounter = PRESSED_COUNTER;
    } 

    button->state = NORMAL;//reset

    if(point_rect_collision(button->rec, mouse)){
        button->state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            button->state = PRESSED;
            button->pressCounter -= GetFrameTime();
        }
    }
}

void updatePanelButton(PanelButton* button, Vector2 mouse)
{
    button->state = NORMAL; 

    if(point_rect_collision(button->rec, mouse)){
        button->state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            button->state = PRESSED;

            if(button->isActive) button->isActive = false;
            else button->isActive = true;
        }
    }
}

void drawActionButton_D(ActionButton_D button)
{
    //base
    DrawRectangleRec(button.rec, button.color);
    if(button.pressCounter < PRESSED_COUNTER){
          //outer 
            DrawLine(button.rec.x, button.rec.y, button.rec.x + button.rec.width, button.rec.y, BLACK); 
            DrawLine(button.rec.x, button.rec.y, button.rec.x, button.rec.y+button.rec.height, BLACK); 

            //inner
            DrawLine(button.rec.x, button.rec.y+1, button.rec.x + button.rec.width, button.rec.y+1, DARKGRAY); 
            DrawLine(button.rec.x+1, button.rec.y, button.rec.x+1, button.rec.y+button.rec.height, DARKGRAY); 

            //outer
            DrawLine(button.rec.x-1, button.rec.y+button.rec.height, button.rec.x + button.rec.width, button.rec.y +button.rec.height, WHITE); 
            DrawLine(button.rec.x + button.rec.width, button.rec.y, button.rec.x + button.rec.width, button.rec.y +button.rec.height, WHITE); 

            //inner
            DrawLine(button.rec.x, button.rec.y+button.rec.height-1, button.rec.x + button.rec.width, button.rec.y +button.rec.height-1, WHITE); 
            DrawLine(button.rec.x + button.rec.width-1, button.rec.y, button.rec.x + button.rec.width-1, button.rec.y +button.rec.height, WHITE); 
    }else{
        if(button.state == NORMAL){
            //top/left border (highlight) (double layer)
            //outer 
            DrawLine(button.rec.x, button.rec.y, button.rec.x + button.rec.width, button.rec.y, WHITE); 
            DrawLine(button.rec.x, button.rec.y, button.rec.x, button.rec.y+button.rec.height, WHITE); 

            //inner
            DrawLine(button.rec.x, button.rec.y+1, button.rec.x + button.rec.width, button.rec.y+1, WHITE); 
            DrawLine(button.rec.x+1, button.rec.y, button.rec.x+1, button.rec.y+button.rec.height, WHITE); 

            //bottom/right border (shadow)
            //outer
            DrawLine(button.rec.x-1, button.rec.y+button.rec.height, button.rec.x + button.rec.width, button.rec.y +button.rec.height, BLACK); 
            DrawLine(button.rec.x + button.rec.width, button.rec.y-1, button.rec.x + button.rec.width, button.rec.y +button.rec.height, BLACK); 

            //inner
            DrawLine(button.rec.x, button.rec.y+button.rec.height-1, button.rec.x + button.rec.width, button.rec.y +button.rec.height-1, DARKGRAY); 
            DrawLine(button.rec.x + button.rec.width-1, button.rec.y, button.rec.x + button.rec.width-1, button.rec.y +button.rec.height, DARKGRAY); 
        }else if(button.state == HOVER){
            //extra outer
            DrawLine(button.rec.x-2, button.rec.y-1, button.rec.x + button.rec.width+1, button.rec.y-1, WHITE); 
            DrawLine(button.rec.x-2, button.rec.y-1, button.rec.x-1, button.rec.y+button.rec.height, WHITE); 

            //outer 
            DrawLine(button.rec.x, button.rec.y, button.rec.x + button.rec.width, button.rec.y, WHITE); 
            DrawLine(button.rec.x, button.rec.y, button.rec.x, button.rec.y+button.rec.height, WHITE); 

            //inner
            DrawLine(button.rec.x, button.rec.y+1, button.rec.x + button.rec.width, button.rec.y+1, WHITE); 
            DrawLine(button.rec.x+1, button.rec.y, button.rec.x+1, button.rec.y+button.rec.height, WHITE); 

            //extra outer
            DrawLine(button.rec.x-2, button.rec.y+button.rec.height+1, button.rec.x + button.rec.width+1, button.rec.y +button.rec.height+1, BLACK); 
            DrawLine(button.rec.x + button.rec.width+1, button.rec.y-2, button.rec.x + button.rec.width+1, button.rec.y +button.rec.height+1, BLACK); 

            //outer
            DrawLine(button.rec.x-1, button.rec.y+button.rec.height, button.rec.x + button.rec.width, button.rec.y +button.rec.height, BLACK); 
            DrawLine(button.rec.x + button.rec.width, button.rec.y-1, button.rec.x + button.rec.width, button.rec.y +button.rec.height, BLACK); 

            //inner
            DrawLine(button.rec.x, button.rec.y+button.rec.height-1, button.rec.x + button.rec.width, button.rec.y +button.rec.height-1, DARKGRAY); 
            DrawLine(button.rec.x + button.rec.width-1, button.rec.y, button.rec.x + button.rec.width-1, button.rec.y +button.rec.height, DARKGRAY); 
        }else if(button.state == PRESSED){
            //outer 
            DrawLine(button.rec.x, button.rec.y, button.rec.x + button.rec.width, button.rec.y, BLACK); 
            DrawLine(button.rec.x, button.rec.y, button.rec.x, button.rec.y+button.rec.height, BLACK); 

            //inner
            DrawLine(button.rec.x, button.rec.y+1, button.rec.x + button.rec.width, button.rec.y+1, DARKGRAY); 
            DrawLine(button.rec.x+1, button.rec.y, button.rec.x+1, button.rec.y+button.rec.height, DARKGRAY); 

            //outer
            DrawLine(button.rec.x-1, button.rec.y+button.rec.height, button.rec.x + button.rec.width, button.rec.y +button.rec.height, WHITE); 
            DrawLine(button.rec.x + button.rec.width, button.rec.y, button.rec.x + button.rec.width, button.rec.y +button.rec.height, WHITE); 

            //inner
            DrawLine(button.rec.x, button.rec.y+button.rec.height-1, button.rec.x + button.rec.width, button.rec.y +button.rec.height-1, WHITE); 
            DrawLine(button.rec.x + button.rec.width-1, button.rec.y, button.rec.x + button.rec.width-1, button.rec.y +button.rec.height, WHITE); 
        }
    }

    int textOffsetX = 0; 
    int textOffsetY = 0;

    if(button.state == PRESSED){
        textOffsetX = 1;
        textOffsetY = 1;
    }if(button.state == HOVER){
        textOffsetX = -1;
        textOffsetY = -1;
    }
    drawText(button.rec, button.label, textOffsetX, textOffsetY, button.textColor);
}

void drawActionButton(ActionButton button)
{
    int textOffsetX = 0;
    int textOffsetY = 0;
    switch(button.state){
        case NORMAL:
            DrawRectangleRec(button.rec, button.color.normal);
            break;
        case HOVER:
            DrawRectangleRec(scaleRec(button.rec, 2), button.color.hover);
            textOffsetX = -1;
            textOffsetY = -1;
            break; 
        case PRESSED:
            DrawRectangleRec(button.rec, button.color.active);
            break;
    } 
    drawText(button.rec, button.label,textOffsetX, textOffsetY, button.textColor);
}

void drawPanelButton(PanelButton button)
{
    if(button.isActive){
        DrawRectangleRec(button.panel.rec, button.panel.color);
        DrawRectangleRec(button.rec, button.color.active);
    }else{
        switch(button.state){
            case NORMAL:
                DrawRectangleRec(button.rec, button.color.normal);
                break;
            case HOVER:
                DrawRectangleRec(button.rec, button.color.hover);
                break;
            case PRESSED:
                DrawRectangleRec(button.rec, button.color.active);
                break;
        }
    }
    drawText(button.rec, button.label, 0, 0, button.textColor);
}

void drawText(Rectangle rec, char* label, int textOffsetX, int textOffsetY, Color color)
{
    //minimum padding
    int padX = 12;
    int padY = 8;

    int availableWidth  = rec.width - padX;
    int availableHeight = rec.height - padY;
    int textLength = strlen(label);

    if(textLength == 0) textLength = 1;

    int sizeBasedOnWidth = availableWidth / textLength;
    int sizeBasedOnHeight = availableHeight; 

    int textSize = (sizeBasedOnWidth < sizeBasedOnHeight) ? sizeBasedOnWidth:sizeBasedOnHeight;

    textSize *= 1.5; //scale up

    //calculate centering coordinates
    int measuredWidth = MeasureText(label, textSize);
    int  x = rec.x + (rec.width - measuredWidth)/2;
    int  y = rec.y + (rec.height - textSize)/2;
    
    DrawText(label, x + textOffsetX, y + textOffsetY,textSize, color);
}

Rectangle scaleRec(Rectangle rec, int scale)
{
    Rectangle rectangle = {
        .x = rec.x - scale,
        .y = rec.y - scale,
        .width = rec.width   + (2 * scale),
        .height = rec.height + (2 * scale)

    };
    return rectangle;
}
