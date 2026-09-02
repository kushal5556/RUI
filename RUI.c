#include "raylib.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

// ----- TODO ----
// -> 

#define WIDTH 800
#define HEIGHT 600

// --- macro ---
#define da_append(array, item)\
    do{\
        if((array).size >= (array).capacity){\
            (array).capacity = (array).capacity == 0 ? 10 : (array).capacity * 2;\
            (array).items = realloc((array).items, sizeof(*(array).items)*(array).capacity);\
            if((array).items == NULL){perror("[DA_APPEND]: Failed to Realloc\n"); exit(1);}\
        }\
        (array).items[(array).size++] = item;\
    }while(0)

// --- global variables --------
const Color SILVER = {192, 192, 192, 255};

#define PRESSED_COUNTER 0.3f //seconds
#define POPUP_TIME 1.0f //seconds
#define DT 1.0f/60.0f
#define MAX_TEXT_INPUT_SIZE 512 //bytes


// --- structs ------
// --- Core structs -----
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
    State state;
}Button;

typedef struct{
    char label[100];
    float x;
    float y;
    Color color;
    float fontSize;
}Label;

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
    DColor color;
    Color textColor;
    State state;

    bool isActive;
    char label[100];
}SelectionButton;

typedef struct{
    char* text;
    Rectangle rec;
    Color color;

    float padY;
    float padX;
    float fontSize;
}Text;

typedef struct{
    Rectangle rec;
    Color color;
}Panel;  

typedef struct{
    Rectangle rec;
    float innerY;
    float barLength;
    Color base;
    DColor color;
    State state; 

    bool isActive; 
}ScrollBar_Y;

typedef struct{
    Rectangle rec;
    float innerX;
    float barLength;
    Color base;
    DColor color;
    State state; 

    bool isActive; 
}ScrollBar_X;

typedef struct{
    char text_input[MAX_TEXT_INPUT_SIZE];
    size_t text_length;

    Rectangle rec;
    Color color;
    Color textColor;

    bool isActive;
    bool pressedEnter;
}TextBox;

// ---- derived structs --------
typedef struct{
    Rectangle rec;
    Color color;
    Color textColor;
    char* text;

    int padX;
    int padY;
    int fontSize;
}TextPanel;

typedef struct{
    ActionButton button;
    Rectangle rec;
    Color color;
    Color textColor;
    char text[512];

    int padX;
    int padY;
    int fontSize;

    bool isActive;
}PopUpPanel;

typedef struct{
    TextPanel panel;
    float time;
}PopUpPanel_Timer;

typedef struct{
    SelectionButton button;
    Panel panel;
}PanelButton_Panel;

typedef struct{
    SelectionButton button;
    TextPanel panel;
}PanelButton_TextPanel;

typedef struct{
    SelectionButton button;
    PopUpPanel panel;
}PanelButton_PopUpPanel;

typedef struct{
    Panel panel;
    ScrollBar_Y scrollBarY;
    ScrollBar_X scrollBarX;

    bool enableScrollY; 
    bool enableScrollX; 
}ScrollPanel;

typedef struct{
    SelectionButton button;
    float baseY;
    float baseX;
}SelectionButton_B; //selection button with base position
//base position are update when window/panel is resized

typedef struct{
    ScrollPanel panel;
    float totalContentHeight;
    float maxContentWidth;

    float padX;
    float padY;

    SelectionButton_B *items;
    size_t size;
    size_t capacity;
}ButtonPanel; 

/// ----- function declaration-----------
Button getButton(float x, float y, float width, float height);
Label getLabel(float x, float y, float fontSize, char* label);
Text getText(float x, float y, float width, float height, float fontSize, float padX, float padY, char* label);
ActionButton getActionButton(float x,  float y,  float width, float height, char* label); //return the default button
ActionButton_D getActionButton_D(float x, float y, float width, float height, char* label); //return the default button_D
SelectionButton getSelectionButton(float x,  float y,  float width, float height, char* label);
Panel getPanel(float x, float y, float width, float height); //return the default panel
TextPanel getTextPanel(float x, float y, float width, float height, const char* text);
PopUpPanel getPopUpPanel(float x, float y, float width, float height, char* text, char* buttonLabel);
PopUpPanel_Timer getPopUpPanel_Timer(float x, float y, float width, float height, char* text);
PanelButton_Panel getPanelButton_Panel(float x, float y, float width, float height, char* label, Panel panel);
PanelButton_TextPanel getPanelButton_TextPanel(float x, float y, float width, float height, char* label, TextPanel panel);
PanelButton_PopUpPanel getPanelButton_PopUpPanel(float x, float y, float width, float height, char* label, PopUpPanel panel);
ScrollBar_Y getScrollBar_Y(float x, float y, float width, float height, float barLength);
ScrollBar_X getScrollBar_X(float x, float y, float width, float height, float barLength);
ScrollPanel getScrollPanel(float x, float y, float width, float height, float barLength);
ButtonPanel getButtonPanel(float x, float y, float width, float height, float baseHeight, float padX, float padY, int buttonCount);
SelectionButton_B getSelectionButton_B(float x, float y, float width, float height, char* label);
TextBox getTextBox(float x, float y, float width, float height);

void freeText(Text* text);
void freeTextPanel(TextPanel* panel);
void freeButtonPanel(ButtonPanel* panel);

void appendButtonPanel(ButtonPanel* panel, SelectionButton_B button, float padX, float padY); 
float getScrollBarY_contentOffset(ScrollBar_Y bar, Rectangle panel, float totalContentHeight);
float getScrollBarX_contentOffset(ScrollBar_X bar, Rectangle panel, float maxContentWidth);

void updateButton(Button* button, Vector2 mouse);
void updateActionButton(ActionButton* button, Vector2 mouse);
void updateActionButton_D(ActionButton_D* button, Vector2 mouse);
void updateSelectionButton(SelectionButton* button, Vector2 mouse);
void updateSelectionButton_B(SelectionButton_B* button, Vector2 mouse);
void updatePopUpPanel(PopUpPanel* panel, Vector2 mouse);
void updatePopUpPanel_Timer(PopUpPanel_Timer* panel);
void updatePanelButton_Panel(PanelButton_Panel* button, Vector2 mouse);
void updatePanelButton_TextPanel(PanelButton_TextPanel* button, Vector2 mouse);
void updatePanelButton_PopUpPanel(PanelButton_PopUpPanel* button, Vector2 mouse);
void updateScrollBar_Y(ScrollBar_Y* bar, Vector2 mouse);
void updateScrollBar_X(ScrollBar_X* bar, Vector2 mouse);
void updateScrollPanel(ScrollPanel* panel, Vector2 mouse);
void updateButtonPanel(ButtonPanel* panel, Vector2 mouse);
void updateTextBox(TextBox* textBox, Vector2 mouse);

void drawButton(Button button);
void drawLabel(Label label);
void drawActionButton_D(ActionButton_D button); 
void drawActionButton(ActionButton button);
void drawSelectionButton(SelectionButton button); 
void drawSelectionButton_B(SelectionButton_B button);
void drawPanel(Panel panel);
void drawTextPanel(TextPanel panel);
void drawPopUpPanel(PopUpPanel panel);
void drawPopUpPanel_Timer(PopUpPanel_Timer panel);
void drawPanelButton_Panel(PanelButton_Panel button);
void drawPanelButton_TextPanel(PanelButton_TextPanel button);
void drawPanelButton_PopUpPanel(PanelButton_PopUpPanel button);
void drawScrollBar_Y(ScrollBar_Y bar);
void drawScrollBar_X(ScrollBar_X bar);
void drawScrollPanel(ScrollPanel panel);
void drawButtonPanel(ButtonPanel panel);
void drawTextBox(TextBox textBox);

void DrawDynamicText(Rectangle rect, const char *text, float maxFontSize, Color textColor, float padX, float padY);
void drawLabelText(Rectangle rec, char* label, int textOffsetX, int textOffsetY, Color color);
void drawText(Text text);
Rectangle scaleRec(Rectangle rec, int scale);

// --- collision function -----------
bool point_rect_collision(Rectangle rec, Vector2 point);
bool circle_point_collision(Vector2 circle, float radius, Vector2 point);


int main(){
    // -------init----------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(WIDTH, HEIGHT, "UI");
    SetTargetFPS(60);

    ActionButton rec = getActionButton(400, 300, 80, 50, "Button");
    ActionButton_D arec = getActionButton_D(500, 400, 80,60, "Click here");

    Panel p = getPanel(200,50,400,150);
    TextPanel tp = getTextPanel(100,10,100,50,"this is just text");
    PopUpPanel pp = getPopUpPanel(300, 10, 300, 150, "this is pop-up! it can handle big strings and dynamically resize", "OK");

    PanelButton_Panel pb = getPanelButton_Panel(50, 100, 80, 30, "Open panel", p);
    PanelButton_TextPanel tpb = getPanelButton_TextPanel(50, 150, 80, 30, "Open text", tp);
    PanelButton_PopUpPanel ppb = getPanelButton_PopUpPanel(50, 200, 80, 30, "Open pop up", pp);

    PopUpPanel_Timer ppt = getPopUpPanel_Timer(400, 10, 50, 50, "Timer Pop");


    ScrollBar_Y sby = getScrollBar_Y(300, 20, 10, 200, 30);
    ScrollBar_X sbx = getScrollBar_X(200, 240, 200, 10, 30);

    ScrollPanel sp = getScrollPanel(100, 200, 300, 200, 30);
    ButtonPanel bp = getButtonPanel(100, 200, 300, 200, 40, 10, 3, 6);

    SelectionButton_B sbb = getSelectionButton_B(100, 200 + (7 * 40), 500, 40, "new button");
    appendButtonPanel(&bp, sbb, 20, 3);

    TextBox tb = getTextBox(500, 300, 250, 50);

    //---------game loop ----------
    while(!WindowShouldClose()){
        if(IsWindowResized()){
            // -> 
        }

        if(PRESSED == rec.state){
            ppt.time = POPUP_TIME;
        }

        if(tb.pressedEnter){
            printf("[TextBox]: %s\n", tb.text_input);
        }

        Vector2 mouse = GetMousePosition();

        updateActionButton(&rec, mouse);  
        updateActionButton_D(&arec, mouse);  

        updatePanelButton_Panel(&pb, mouse);
        updatePanelButton_TextPanel(&tpb, mouse);
        updatePanelButton_PopUpPanel(&ppb, mouse);

        updatePopUpPanel_Timer(&ppt);

        updateScrollBar_Y(&sby, mouse);
        updateScrollBar_X(&sbx, mouse);

        updateScrollPanel(&sp, mouse);

        updateButtonPanel(&bp, mouse);

        updateTextBox(&tb, mouse);

        // --- clear and draw -----
        BeginDrawing();
        ClearBackground(SKYBLUE);

        drawActionButton(rec);
        drawActionButton_D(arec);

        // drawPanel(p);
        // drawPopUpPanel(pp);
        // drawTextPanel(tp);

        drawPopUpPanel_Timer(ppt);

        drawPanelButton_Panel(pb);
        drawPanelButton_TextPanel(tpb);
        drawPanelButton_PopUpPanel(ppb);

        drawScrollBar_Y(sby);
        drawScrollBar_X(sbx);

        // drawScrollPanel(sp);
        drawButtonPanel(bp);

        drawTextBox(tb);

        EndDrawing();
    }

    // -- close----
    freeTextPanel(&tp);
    freeButtonPanel(&bp);
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

//------ get panel/buttons ---------
Button getButton(float x, float y, float width, float height)
{
    Button button = {
        .rec = (Rectangle){x,y,width,height},
        .color = (DColor){
            .normal = SILVER,
            .hover = GRAY,
            .active = DARKGRAY
        },
        .state = NORMAL
    };
    return button;
}

Label getLabel(float x, float y, float fontSize, char* label)
{
    Label lab = {
        .x = x, 
        .y = y, 
        .color = BLACK,
        .fontSize = fontSize,
    };
    strcpy(lab.label, label);
    return lab;
}

Text getText(float x, float y, float width, float height, float fontSize, float padX, float padY, char* label)
{
    Text text = {
        .text = strdup(label),
        .color = BLACK,
        .rec = (Rectangle){x,y,width,height},
        .padY = padY,
        .padX = padX,
        .fontSize = fontSize
    };
    return text;
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

SelectionButton getSelectionButton(float x,  float y,  float width, float height, char* label)
{
    SelectionButton button = {
        .rec = (Rectangle){x,y,width,height},
        .color = (DColor){
            .normal  = SILVER,
            .hover   = LIGHTGRAY,
            .active  = GRAY
        },
        .textColor = BLACK,
        .state = NORMAL,
        .isActive = false
    };
    strcpy(button.label, label);

    return button; 
}

SelectionButton_B getSelectionButton_B(float x, float y, float width, float height, char* label)
{
    SelectionButton_B button = {
        .button = getSelectionButton(x, y, width, height, label),
        .baseX = x,
        .baseY = y
    };
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

TextPanel getTextPanel(float x, float y, float width, float height, const char* text)
{
    TextPanel panel = {
        .rec = (Rectangle){x, y, width, height},
        .color = SILVER,
        .textColor = BLACK,
        .text = strdup(text),
        .padX = 12,
        .padY = 8,
        .fontSize  = 32
    };
    return panel;
}

PopUpPanel getPopUpPanel(float x, float y, float width, float height, char* text, char* buttonLabel)
{
    float bW = 15*width/100;
    float bH = 15*height/100;

    PopUpPanel panel = {
        .rec = (Rectangle){x, y, width, height},
        .color = SILVER,
        .textColor = BLACK,
        .padX = 12,
        .padY = 8,
        .fontSize  = 32,
        .isActive = false,
        .button = getActionButton(x + ((width/2) - (bW/2)), y + (height - (bH*1.5)), bW, bH, buttonLabel)   
    };
    strcpy(panel.text, text);
    return panel;
}

PopUpPanel_Timer getPopUpPanel_Timer(float x, float y, float width, float height, char* text)
{
    PopUpPanel_Timer panel = {
        .panel = getTextPanel(x,y,width,height,text),
        .time = POPUP_TIME
    };
    return panel;
}

PanelButton_Panel getPanelButton_Panel(float x, float y, float width, float height, char* label, Panel panel)
{
    PanelButton_Panel button = {
        .button = getSelectionButton(x, y, width, height, label),
        .panel = panel
    };
    strcpy(button.button.label, label);
    return button;
}

PanelButton_TextPanel getPanelButton_TextPanel(float x, float y, float width, float height, char* label, TextPanel panel)
{
    PanelButton_TextPanel button = {
        .button = getSelectionButton(x, y, width, height, label),
        .panel = panel
    };
    strcpy(button.button.label, label);
    return button;
}

PanelButton_PopUpPanel getPanelButton_PopUpPanel(float x, float y, float width, float height, char* label, PopUpPanel panel)
{
    PanelButton_PopUpPanel button = {
        .button = getSelectionButton(x, y, width, height, label),
        .panel = panel
    };
    strcpy(button.button.label, label);
    return button;
}

ScrollBar_Y getScrollBar_Y(float x, float y, float width, float height, float barLength)
{
    ScrollBar_Y sb = {
        .rec = (Rectangle){x, y, width, height},
        .innerY = y,
        .barLength = barLength > height ? height:barLength,
        .base = SILVER,
        .color = (DColor){
            .normal = GRAY,
            .hover = WHITE,
            .active = GREEN
        },
        .state = NORMAL,
        .isActive = false
    };
    return sb;
}

ScrollBar_X getScrollBar_X(float x, float y, float width, float height, float barLength)
{
    ScrollBar_X sb = {
        .rec = (Rectangle){x, y, width, height},
        .innerX = x,
        .barLength = barLength > width ? width:barLength,
        .base = SILVER,
        .color = (DColor){
            .normal = GRAY,
            .hover = WHITE,
            .active = GREEN
        },
        .state = NORMAL,
        .isActive = false
    };
    return sb;
}

ScrollPanel getScrollPanel(float x, float y, float width, float height, float barLength)
{
    int sbW = 8;
    int sbPad = 5;

    ScrollPanel panel = {
        .panel = getPanel(x, y, width, height),
        .scrollBarX = getScrollBar_X(x, y+height, width,sbW, barLength),
        .scrollBarY = getScrollBar_Y(x+width, y, sbW, height, barLength),
        .enableScrollY = true,
        .enableScrollX = true
    };
    panel.panel.color = DARKGRAY;

    return panel;
}

ButtonPanel getButtonPanel(float x, float y, float width, float height, float baseHeight, float padX, float padY, int buttonCount)
{
    ButtonPanel panel = {
        .panel = getScrollPanel(x, y, width, height, 30),
        .items = malloc(sizeof(SelectionButton_B)*buttonCount),
        .size = buttonCount,
        .capacity = buttonCount,
        .totalContentHeight = 0.0f,
        .maxContentWidth = 0.0f,
        .padY = padY,
        .padX = padX
    };

    float totalHeight = 0.0f;
    float maxWidth = -9999.0f;
    int padXLeft = padX;
    int bW = width-(padXLeft) - panel.panel.scrollBarY.rec.width;
    int bH = baseHeight > height ? height:baseHeight;
    int padTop = padY;

    for(int i = 0; i < buttonCount; i++){
        panel.items[i] = getSelectionButton_B(x + padXLeft, y + padTop + (i*bH) + (i*padTop), bW, bH, "");

        panel.items[i].baseY += padY;
        panel.items[i].baseX += padX;

        totalHeight +=bH+padTop;

        if(maxWidth + padXLeft < panel.items[i].button.rec.width){
            maxWidth = panel.items[i].button.rec.width + padXLeft;
        }
    }
    panel.totalContentHeight = totalHeight;
    panel.maxContentWidth    = maxWidth;

    panel.panel.enableScrollY = false;
    panel.panel.enableScrollX = false;

    if(totalHeight > height){
        panel.panel.enableScrollY = true;
    }
    if(maxWidth > width){
        panel.panel.enableScrollX = true;
    }
    return panel;
}

TextBox getTextBox(float x, float y, float width, float height)
{
    TextBox tb = {
        .text_length = 0, 
        .rec = (Rectangle){x,y,width,height},
        .color = SILVER,
        .textColor = BLACK,
        .isActive = false,
        .pressedEnter = false
    };
    tb.text_input[0] = '\0';

    return tb;
}


// ------- free memory------------
void freeText(Text* text)
{
    free(text->text);
}

void freeTextPanel(TextPanel* panel)
{
    free(panel->text);
}

void freeButtonPanel(ButtonPanel* panel)
{
    free(panel->items);
}

void appendButtonPanel(ButtonPanel* panel, SelectionButton_B button, float padX, float padY)
{
    if(panel->size > 0){
        button.button.rec.x = panel->panel.panel.rec.x + padX;
        button.button.rec.y = (panel->items[panel->size - 1].baseY + panel->items[panel->size - 1].button.rec.height) + padY;

        button.baseX = button.button.rec.x;
        button.baseY = button.button.rec.y;
    }else{
        if(panel->panel.panel.rec.width <= 0.0f && 
            panel->panel.panel.rec.height <= 0.0f){
            perror("[APPEND_BUTTON_PANEL]: Un-initialized Panel\n");
            exit(1);
        }

        button.button.rec.x = panel->panel.panel.rec.x + padX;
        button.button.rec.y = panel->panel.panel.rec.y + padY;

        button.baseX = button.button.rec.x;
        button.baseY = button.button.rec.y;

        panel->totalContentHeight = 0.0f; 
        panel->maxContentWidth = 0.0f; 
        panel->padY = padY;
        panel->padX = padX;
        panel->panel.enableScrollY = false;
        panel->panel.enableScrollX = false;
    }
    
    da_append(*panel, button); 
    
    panel->totalContentHeight += button.button.rec.height + padY;
    if(panel->totalContentHeight > panel->panel.panel.rec.height){
        panel->panel.enableScrollY = true;
    }

    if(panel->maxContentWidth < button.button.rec.width + padX){
        panel->maxContentWidth = button.button.rec.width + padX;
        if(panel->maxContentWidth > panel->panel.panel.rec.width){
            panel->panel.enableScrollX = true;
        }
    }
}

float getScrollBarY_contentOffset(ScrollBar_Y bar, Rectangle panel, float totalContentHeight)
{
    float trackHeight = bar.rec.height;
    float trackTop    = bar.rec.y;
    float thumbY      = bar.innerY;
    float thumbHeight = bar.barLength;
    float panelHeight = panel.height;

    float maxTravel = trackHeight - thumbHeight;
    float maxContentOffset = totalContentHeight - panelHeight;

    float scrollRatio = (thumbY - trackTop) / maxTravel; 

    if(scrollRatio < 0.0f) scrollRatio = 0.0f;
    if(scrollRatio > 1.0f) scrollRatio = 1.0f;

    float contentOffset = scrollRatio * maxContentOffset;
    return contentOffset;
}

float getScrollBarX_contentOffset(ScrollBar_X bar, Rectangle panel, float maxContentWidth)
{
    float trackWidth  = bar.rec.width;
    float trackLeft   = bar.rec.x;
    float thumbX      = bar.innerX;
    float thumbWidth  = bar.barLength;
    float panelWidth  = panel.width;

    float maxTravel = trackWidth - thumbWidth;
    float maxContentOffset = maxContentWidth - panelWidth;

    float scrollRatio = (thumbX - trackLeft) / maxTravel; 

    if(scrollRatio < 0.0f) scrollRatio = 0.0f;
    if(scrollRatio > 1.0f) scrollRatio = 1.0f;

    float contentOffset = scrollRatio * maxContentOffset;
    return contentOffset;
}


// -------- update panels/buttons ---------
void updateButton(Button* button, Vector2 mouse)
{
    button->state = NORMAL;
    if(point_rect_collision(button->rec, mouse)){
        button->state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            button->state = PRESSED;
        }
    }
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
    if(button->pressCounter != PRESSED_COUNTER || button->pressCounter < PRESSED_COUNTER){
        button->pressCounter -= DT;
        if(button->pressCounter <= 0.0f) button->pressCounter = PRESSED_COUNTER;
    } 

    button->state = NORMAL;//reset

    if(point_rect_collision(button->rec, mouse)){
        button->state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            button->state = PRESSED;
            button->pressCounter -= DT;
        }
    }
}

void updateSelectionButton(SelectionButton* button, Vector2 mouse)
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

void updateSelectionButton_B(SelectionButton_B* button, Vector2 mouse)
{
    updateSelectionButton(&button->button, mouse);
}

void updatePopUpPanel(PopUpPanel* panel, Vector2 mouse)
{
    if(!panel->isActive) return;

    panel->button.state = NORMAL;

    if(point_rect_collision(panel->button.rec, mouse)){
        panel->button.state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            panel->button.state = PRESSED;
            panel->isActive = false;
        }
    }
}

void updatePopUpPanel_Timer(PopUpPanel_Timer* panel)
{
    if(panel->time <= 0.0f) return;
    panel->time -= DT;
}

void updatePanelButton_Panel(PanelButton_Panel* button, Vector2 mouse)
{

    updateSelectionButton(&button->button, mouse);
}

void updatePanelButton_TextPanel(PanelButton_TextPanel* button, Vector2 mouse)
{
    updateSelectionButton(&button->button, mouse);
}

void updatePanelButton_PopUpPanel(PanelButton_PopUpPanel* button, Vector2 mouse)
{
    if(button->panel.isActive){
        updatePopUpPanel(&button->panel, mouse);

        if(!button->panel.isActive){
            button->button.isActive = false;
            button->panel.isActive = false;
        }
    }

    button->button.state = NORMAL; 

    if(point_rect_collision(button->button.rec, mouse)){
        button->button.state = HOVER;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            button->button.state = PRESSED;

            if(button->button.isActive){
                button->button.isActive = false;
                button->panel.isActive = false;
            } 
            else {
                button->button.isActive = true;
                button->panel.isActive = true;
            }
        }
    }
}

void updateScrollBar_Y(ScrollBar_Y* bar, Vector2 mouse)
{
    float y = bar->rec.y;
    float h = bar->rec.height;

    float innerY = bar->innerY;
    Rectangle barRec = {bar->rec.x, innerY, bar->rec.width, bar->barLength};

    bar->state = NORMAL;

    if(!bar->isActive){
        if(point_rect_collision(barRec, mouse)){
            bar->state = HOVER;
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                bar->state = PRESSED;
                bar->isActive = true;
            }
        }
    }

    if(bar->isActive){
        bar->state = PRESSED;
        Vector2 delta = GetMouseDelta();

        if(innerY >= y && (innerY+bar->barLength) <= (y+h)){
            innerY += delta.y;
        }

        if(innerY < y) { bar->innerY = y; }
        else if((innerY+bar->barLength) > (h+y)){ bar->innerY = (y+h)-bar->barLength;}
        else {bar->innerY = innerY;}


        if(IsMouseButtonUp(MOUSE_BUTTON_LEFT)){
            bar->isActive = false;
        }
    }
}

void updateScrollBar_X(ScrollBar_X* bar, Vector2 mouse)
{
    float x = bar->rec.x;
    float w = bar->rec.width;

    float innerX = bar->innerX;
    Rectangle barRec = {innerX, bar->rec.y, bar->barLength, bar->rec.height};

    bar->state = NORMAL;

    if(!bar->isActive){
        if(point_rect_collision(barRec, mouse)){
            bar->state = HOVER;
            if(IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
                bar->state = PRESSED;
                bar->isActive = true;
            }
        }
    }

    if(bar->isActive){
        bar->state = PRESSED;
        Vector2 delta = GetMouseDelta();

        if(innerX >= x && (innerX+bar->barLength) <= (x+w)){
            innerX += delta.x;
        }

        if(innerX < x) { bar->innerX = x; }
        else if((innerX+bar->barLength) > (w+x)){ bar->innerX = (x+w)-bar->barLength;}
        else {bar->innerX = innerX;}


        if(IsMouseButtonUp(MOUSE_BUTTON_LEFT)){
            bar->isActive = false;
        }
    }
}

void updateScrollPanel(ScrollPanel* panel, Vector2 mouse)
{
    if(panel->enableScrollX){
        updateScrollBar_X(&panel->scrollBarX, mouse);
    }
    if(panel->enableScrollY){
        updateScrollBar_Y(&panel->scrollBarY, mouse);
    }
}

void updateButtonPanel(ButtonPanel* panel, Vector2 mouse)
{
    updateScrollPanel(&panel->panel, mouse);

    Rectangle panelRec = panel->panel.panel.rec;
    float contentHeight   = panel->totalContentHeight + (panel->padY*2);
    float maxContentWidth = panel->maxContentWidth; 

    float contentOffsetY = 0.0f;
    float contentOffsetX = panel->padX; 
    if(panel->panel.enableScrollY){
        contentOffsetY = getScrollBarY_contentOffset(panel->panel.scrollBarY, panelRec, contentHeight);
    }
    if(panel->panel.enableScrollX){
        contentOffsetX += getScrollBarX_contentOffset(panel->panel.scrollBarX, panelRec, maxContentWidth);
    }

    for(int i = 0; i < panel->size; i++){
        panel->items[i].button.rec.y = panel->items[i].baseY - contentOffsetY;
        panel->items[i].button.rec.x = panel->items[i].baseX - contentOffsetX;

        //check if whole/part of button is inside the panel
        if(CheckCollisionRecs(panel->items[i].button.rec, panelRec)){
            
            //ensure the mouse is inside the panel (prevent invisible touch)
            if(point_rect_collision(panelRec, mouse)) {
                updateSelectionButton_B(&panel->items[i], mouse);
            } else {
                //if outside reset state 
                panel->items[i].button.state = NORMAL;
            }

            if(panel->items[i].button.isActive){
                for(int j = 0; j < panel->size; j++){
                    if(i == j) continue;
                    panel->items[j].button.isActive = false;
                }
            }
        }
    }
}

void updateTextBox(TextBox* textBox, Vector2 mouse)
{
    if(textBox->pressedEnter){
        //reset the text box (use it before reseting)
        textBox->text_length = 0;
        textBox->text_input[textBox->text_length] = '\0'; 
        textBox->pressedEnter = false;
    }

    if(point_rect_collision(textBox->rec, mouse)){
       if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        textBox->isActive = true;
       } 
    }else{
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            textBox->isActive = false;
        }
    }

    if(textBox->isActive){
        int key = GetCharPressed();
        if(textBox->text_length >= MAX_TEXT_INPUT_SIZE-1) return;

        if(key >= 32 && key <= 126){
            textBox->text_input[textBox->text_length++] = key;
            textBox->text_input[textBox->text_length] = '\0';
        }

        if(IsKeyPressed(KEY_BACKSPACE)){
           if(textBox->text_length > 0){
                textBox->text_length--;
                textBox->text_input[textBox->text_length] = '\0';
           } 
        }

        if(IsKeyPressed(KEY_ENTER)){
            textBox->pressedEnter = true;
        }
    }
}

// --------- draw panels/buttons --------------
void drawButton(Button button)
{
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

void drawLabel(Label label)
{
    DrawText(label.label, label.x, label.y, label.fontSize, label.color);    
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

    if(button.state == PRESSED || button.pressCounter < PRESSED_COUNTER){
        textOffsetX = 1;
        textOffsetY = 1;
    }else if(button.state == HOVER){
        textOffsetX = -1;
        textOffsetY = -1;
    }

    drawLabelText(button.rec, button.label, textOffsetX, textOffsetY, button.textColor);
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
    drawLabelText(button.rec, button.label,textOffsetX, textOffsetY, button.textColor);
}

void drawSelectionButton(SelectionButton button)
{
    if(button.isActive){
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
    drawLabelText(button.rec, button.label, 0, 0, button.textColor);
}

void drawSelectionButton_B(SelectionButton_B button)
{
    drawSelectionButton(button.button);
}

void drawPanel(Panel panel)
{
    DrawRectangleRec(panel.rec, panel.color);
}

void drawTextPanel(TextPanel panel)
{
    DrawRectangleRec(panel.rec, panel.color);
    DrawDynamicText(panel.rec, panel.text, panel.fontSize, panel.textColor, panel.padX, panel.padY);
}

void drawPopUpPanel(PopUpPanel panel)
{
    if(!panel.isActive) return;
    DrawRectangleRec(panel.rec, panel.color);
    DrawDynamicText(panel.rec, panel.text, panel.fontSize, panel.textColor, panel.padX, panel.padY);

    DrawRectangleRec(scaleRec(panel.button.rec, 2), BLACK);
    drawActionButton(panel.button);
}

void drawPopUpPanel_Timer(PopUpPanel_Timer panel)
{
    if(panel.time <= 0.0f) return;
    DrawRectangleRec(panel.panel.rec, panel.panel.color);
    DrawDynamicText(panel.panel.rec, panel.panel.text, panel.panel.fontSize, panel.panel.textColor, panel.panel.padX, panel.panel.padY);
}

void drawPanelButton_Panel(PanelButton_Panel button)
{
    if(button.button.isActive){
        drawPanel(button.panel);
        DrawRectangleRec(button.button.rec, button.button.color.active);
    }else{
        switch(button.button.state){
            case NORMAL:
                DrawRectangleRec(button.button.rec, button.button.color.normal);
                break;
            case HOVER:
                DrawRectangleRec(button.button.rec, button.button.color.hover);
                break;
            case PRESSED:
                DrawRectangleRec(button.button.rec, button.button.color.active);
                break;
        }
    }
    drawLabelText(button.button.rec, button.button.label, 0, 0, button.button.textColor);
}

void drawPanelButton_TextPanel(PanelButton_TextPanel button)
{
    if(button.button.isActive){
        drawTextPanel(button.panel);
        DrawRectangleRec(button.button.rec, button.button.color.active);
    }else{
        switch(button.button.state){
            case NORMAL:
                DrawRectangleRec(button.button.rec, button.button.color.normal);
                break;
            case HOVER:
                DrawRectangleRec(button.button.rec, button.button.color.hover);
                break;
            case PRESSED:
                DrawRectangleRec(button.button.rec, button.button.color.active);
                break;
        }
    }
    drawLabelText(button.button.rec, button.button.label, 0, 0, button.button.textColor);
}

void drawPanelButton_PopUpPanel(PanelButton_PopUpPanel button)
{
    if(button.button.isActive){
        drawPopUpPanel(button.panel);
        DrawRectangleRec(button.button.rec, button.button.color.active);
    }else{
        switch(button.button.state){
            case NORMAL:
                DrawRectangleRec(button.button.rec, button.button.color.normal);
                break;
            case HOVER:
                DrawRectangleRec(button.button.rec, button.button.color.hover);
                break;
            case PRESSED:
                DrawRectangleRec(button.button.rec, button.button.color.active);
                break;
        }
    }
    drawLabelText(button.button.rec, button.button.label, 0, 0, button.button.textColor);
}

void drawScrollBar_Y(ScrollBar_Y bar)
{
    DrawRectangleRec(bar.rec, bar.base); 

    Rectangle barRec = {bar.rec.x, bar.innerY, bar.rec.width, bar.barLength};
    switch(bar.state){
        case NORMAL:
            DrawRectangleRec(barRec, bar.color.normal); 
            break;
        case HOVER:
            DrawRectangleRec(barRec, bar.color.hover); 
            break;
        case PRESSED:
            DrawRectangleRec(barRec, bar.color.active); 
            break;
    }
}

void drawScrollBar_X(ScrollBar_X bar)
{
    DrawRectangleRec(bar.rec, bar.base); 

    Rectangle barRec = {bar.innerX, bar.rec.y, bar.barLength, bar.rec.height};
    switch(bar.state){
        case NORMAL:
            DrawRectangleRec(barRec, bar.color.normal); 
            break;
        case HOVER:
            DrawRectangleRec(barRec, bar.color.hover); 
            break;
        case PRESSED:
            DrawRectangleRec(barRec, bar.color.active); 
            break;
    }
}

void drawScrollPanel(ScrollPanel panel)
{
    drawPanel(panel.panel);
    if(panel.enableScrollX){
        drawScrollBar_X(panel.scrollBarX);
    }
    if(panel.enableScrollY){
        drawScrollBar_Y(panel.scrollBarY);
    }
}

void drawButtonPanel(ButtonPanel panel)
{
    drawScrollPanel(panel.panel);
    BeginScissorMode(
        (int)panel.panel.panel.rec.x, 
        (int)panel.panel.panel.rec.y, 
        (int)panel.panel.panel.rec.width, 
        (int)panel.panel.panel.rec.height
    );

    for(int i = 0; i < panel.size; i++){
        drawSelectionButton_B(panel.items[i]);
    }
    EndScissorMode();
}

void drawTextBox(TextBox textBox)
{
    int padX = 10;
    int padY = 5;
    int fontSize = textBox.rec.height - (2*padY);
    int textWidth = MeasureText(textBox.text_input, fontSize);

    DrawRectangleRec(textBox.rec, textBox.color);
    BeginScissorMode(
        textBox.rec.x + padX,
        textBox.rec.y,
        textBox.rec.width,
        textBox.rec.height
    );

    static float textOffsetX = 0.0f;
    if(textWidth >= (textBox.rec.width - padX)){textOffsetX = textWidth - (textBox.rec.width - (2*padX));}
    else textOffsetX = 0.0f;

    DrawText(textBox.text_input, textBox.rec.x + padX - textOffsetX, textBox.rec.y + padY, fontSize, textBox.textColor);
    EndScissorMode(); 


    static bool Blink = true;
    static float BlinkTime = 0.0f;
    BlinkTime += DT;

    if(BlinkTime >= 0.5f){
        BlinkTime = 0.0f;
        if(!Blink) Blink = true;
        else Blink = false;
    }

    if(textBox.isActive && Blink){
        float cursor_x = textBox.rec.x + padX + textWidth;
        if(cursor_x > textBox.rec.x + (textBox.rec.width - padX)) cursor_x = textBox.rec.x + textBox.rec.width - padX;

        DrawLineEx((Vector2){cursor_x, 
                textBox.rec.y + (padY/2)},
                (Vector2){cursor_x, 
                textBox.rec.y + textBox.rec.height - padY}, 3, textBox.textColor
        );
    }
}


void drawText(Text text){
    DrawDynamicText(text.rec, text.text, text.fontSize, text.color, text.padX, text.padY);
}

void drawLabelText(Rectangle rec, char* label, int textOffsetX, int textOffsetY, Color color)
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

void DrawDynamicText(Rectangle rect, const char *text, float maxFontSize, Color textColor, float padX, float padY) 
{
    float s = maxFontSize;
    char lines[64][256];
    int lineCount = 0;
    
    float innerWidth  = rect.width  - (padX * 2.0f);
    float innerHeight = rect.height - (padY * 2.0f);
    
    // Iterative Auto-Sizing Loop
    while (s > 4.0f) {
        lineCount = 0;
        memset(lines, 0, sizeof(lines));
        
        char textCopy[1024];
        strncpy(textCopy, text, sizeof(textCopy));
        
        char *word = strtok(textCopy, " ");
        char currentLine[256] = "";
        
        while (word != NULL) {
            char testLine[256];
            if (strlen(currentLine) > 0) {
                snprintf(testLine, sizeof(testLine), "%s %s", currentLine, word);
            } else {
                snprintf(testLine, sizeof(testLine), "%s", word);
            }
            
            int lineWidth = MeasureText(testLine, (int)s);
            
            if (lineWidth > innerWidth) {
                if (lineCount < 64) {
                    strcpy(lines[lineCount++], currentLine);
                }
                snprintf(currentLine, sizeof(currentLine), "%s", word);
            } else {
                strcpy(currentLine, testLine);
            }
            word = strtok(NULL, " ");
        }
        if (strlen(currentLine) > 0 && lineCount < 64) {
            strcpy(lines[lineCount++], currentLine);
        }
        
        float lineHeight = s * 1.2f; 
        float totalBlockHeight = lineCount * lineHeight;
        
        // Check if block fits within the restricted inner height bounds
        if (totalBlockHeight > innerHeight) {
            s -= 1.0f;
        } else {
            break; 
        }
    }
    
    //  Compute Centering Offsets constrained inside the padded inner area
    float lineHeight = s * 1.2f;
    float totalBlockHeight = lineCount * lineHeight;
    float startY = rect.y + padY + (innerHeight - totalBlockHeight) / 2.0f;
    
    // Draw Each Line Centered Horizontally Within the Inner Bounds
    for (int i = 0; i < lineCount; i++) {
        int lineWidth = MeasureText(lines[i], (int)s);
        
        float startX = rect.x + padX + (innerWidth - lineWidth) / 2.0f;
        float currentY = startY + (i * lineHeight);
        
        DrawText(lines[i], (int)startX, (int)currentY, (int)s, textColor);
    }
}