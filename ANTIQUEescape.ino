#include <M5Cardputer.h>
auto& d = M5Cardputer.Display;

enum GameScene { SCENE_INTRO, SCENE_LOCK, SCENE_ROOM, SCENE_WIN };
GameScene currentScene = SCENE_INTRO;

int combination[3] = {0, 0, 0};
int secretCode[3] = {8, 1, 8};
int activeSlot = 0;
int tiltCooldown = 0;
int selectedItem = 0;
const int totalItems = 5;

String itemNames[totalItems] = {
    "1. Clock",
    "2. Dusty Painting",
    "3. Iron Safe",
    "4. Sheet Music",
    "5. Old Bookshelf"
};

int musicBoxBeats = 1;

void setup() {
    auto cfg = M5.config();
    M5Cardputer.begin(cfg, true);
    M5.Imu.begin();
    d.setRotation(1);
    d.setBrightness(50);
    drawIntroScreen();
}

void loop() {
    M5Cardputer.update();

    if (currentScene == SCENE_INTRO) {
        if (M5Cardputer.Keyboard.isPressed()) {
            Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
            if (status.enter || status.space) {
                M5Cardputer.Speaker.tone(300, 100);
                delay(120);
                M5Cardputer.Speaker.tone(200, 150);
                currentScene = SCENE_LOCK;
                drawLockScreen();
                delay(300);
            }
        }
        return;
    }

    if (currentScene == SCENE_WIN) return;

    if (M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        if (status.tab) {
            M5Cardputer.Speaker.tone(1200, 40);
            if (currentScene == SCENE_LOCK) {
                currentScene = SCENE_ROOM;
                drawRoomScreen(); 
            } else {
                currentScene = SCENE_LOCK;
                drawLockScreen();
            }
            delay(250);
        }
    }

    if (currentScene == SCENE_LOCK) {
        handleLockInput();
    } else {
        handleRoomInput();
    }
    delay(30);
}

void drawIntroScreen() {
    d.fillScreen(BLACK);
    d.setTextColor(ORANGE);
    d.setTextSize(2);
    d.setCursor(25, 15);
    d.print("Antique Escape");

    d.setTextSize(1);
    d.setTextColor(WHITE);
    d.setCursor(15, 50);
    d.print("Trapped in an antique store");
    d.setCursor(15, 65);
    d.print("on Halloween! Crack the lock");
    d.setCursor(15, 80);
    d.print("or no trick or treat! ):");

    d.setTextColor(YELLOW);
    d.setCursor(35, 110);
    d.print("Press ENTER to begin");
}

void drawLockScreen() {
    d.fillScreen(BLACK);
    d.setTextColor(ORANGE);
    d.setTextSize(1);
    d.setCursor(10, 10);
    d.print("LOCK [Tab to switch]");
    d.setTextColor(LIGHTGREY);
    d.setCursor(10, 115);
    d.print("Tilt: Digits | < >: Slots | OK: Unlock");
    updateLockDisplay();
}

void updateLockDisplay() {
    for (int i = 0; i < 3; i++) {
        uint16_t boxColor = (i == activeSlot) ? GREEN : DARKGREY;
        d.fillRect(40 + (i * 55), 45, 45, 50, BLACK);
        d.drawRect(40 + (i * 55), 45, 45, 50, boxColor);
        d.setTextSize(3);
        d.setCursor(55 + (i * 55), 57);
        d.setTextColor(boxColor);
        d.print(combination[i]);
    }
}

void drawRoomScreen() {
    d.fillScreen(BLACK);
    d.setTextColor(ORANGE);
    d.setTextSize(1);
    d.setCursor(10, 5);
    d.print("Antique Shop [TAB: Lock | H: Hint]");
    d.drawRect(8, 20, 115, 78, DARKGREY);  
    d.drawRect(127, 20, 105, 78, DARKGREY); 
    d.drawRect(8, 101, 224, 28, DARKGREY);  
    updateRoomDisplay();
}

void updateRoomDisplay() {
    d.fillRect(10, 22, 111, 74, BLACK);
    for (int i = 0; i < totalItems; i++) {
        uint16_t textColor = (i == selectedItem) ? YELLOW : LIGHTGREY;
        d.setCursor(12, 24 + (i * 14));
        d.setTextSize(1);
        d.setTextColor(textColor);
        d.print(i == selectedItem ? ">" : " ");
        d.print(itemNames[i]);
    }
    drawItemGraphic(selectedItem);
}

void drawItemGraphic(int index) {
    d.fillRect(128, 21, 103, 76, BLACK);
    d.fillRect(9, 102, 222, 26, BLACK);

    if (index == 0) { // clock
        d.fillRect(165, 30, 30, 55, 0x61A0);
        d.fillCircle(180, 50, 12, WHITE);
        d.drawPixel(180, 39, BLACK); d.drawPixel(191, 50, BLACK);
        d.drawPixel(180, 61, BLACK); d.drawPixel(169, 50, BLACK);
        d.drawLine(180, 50, 180, 42, BLACK); 
        d.drawLine(180, 50, 184, 45, BLACK); 
        d.fillCircle(180, 70, 4, YELLOW); 
        d.setCursor(12, 110);
        d.setTextColor(YELLOW);
        d.print("The clock must be broken...");
    }
    else if (index == 1) { // painting
        d.fillRect(160, 32, 40, 50, 0x4800);
        d.fillRect(164, 36, 32, 42, BLUE);
        for(int i = 0; i < 8; i++) d.drawPixel(172 + (i * 3), 55, WHITE);
        d.setCursor(12, 110);
        d.setTextColor(CYAN);
        d.print("Empty ships sail across the sea...");
    }
    else if (index == 2) { // safe
        d.fillRect(162, 32, 36, 50, LIGHTGREY);
        d.fillCircle(180, 57, 10, DARKGREY);
        d.fillCircle(180, 57, 3, YELLOW);
        d.setCursor(170, 35);
        d.setTextColor(RED);
        d.print("GR8");
        d.setCursor(12, 110);
        d.setTextColor(GREEN);
        d.print("Graffiti on the safe");
    }
    else if (index == 3) { // music
        d.fillRect(133, 25, 93, 68, 0xF676); 
        for (int y = 45; y <= 65; y += 6) d.drawFastHLine(138, y, 83, DARKGREY);
        d.drawFastVLine(138, 45, 20, BLACK); d.drawFastVLine(165, 45, 20, BLACK);
        d.drawFastVLine(192, 45, 20, BLACK); d.drawFastVLine(220, 45, 20, BLACK);
        d.drawCircle(146, 60, 2, BLACK); 
        d.drawCircle(174, 48, 2, BLACK); 
        d.drawCircle(201, 52, 2, BLACK);
        d.setCursor(140, 78);
        d.setTextColor(MAGENTA);
        d.print("BEAT: "); d.print(musicBoxBeats);
        d.setCursor(12, 110);
        d.setTextColor(MAGENTA);
        d.print("Sheet music in 4/4 time");             
    }
    else if (index == 4) {
        d.fillRect(145, 30, 70, 55, 0x4800);
        // Book 1 (Safe)
        d.fillRect(151, 36, 9, 42, RED);
        d.setCursor(153, 45); d.setTextColor(WHITE); d.print("1");
        d.setCursor(153, 55); d.print("S");
        // Book 2 (Clock) 
        d.fillRect(161, 36, 9, 42, BLUE);
        d.setCursor(163, 45); d.setTextColor(WHITE); d.print("2");
        d.setCursor(163, 55); d.print("C");
        // Book 3 (Painting)
        d.fillRect(171, 36, 9, 42, GREEN);
        d.setCursor(173, 45); d.setTextColor(WHITE); d.print("3");
        d.setCursor(173, 55); d.print("P");
        d.fillRect(181, 36, 9, 42, YELLOW);

        d.setCursor(12, 110);
        d.setTextColor(LIGHTGREY);
        d.print("Bookshelf spines show a clue...");
    }
}

void handleLockInput() {
    M5.Imu.update();
    auto imu_data = M5.Imu.getImuData();
    float tiltX = imu_data.accel.x;

    tiltCooldown++;
    if (tiltCooldown > 4) {
        if (tiltX > 0.3) {
            combination[activeSlot] = (combination[activeSlot] + 9) % 10;
            tiltCooldown = 0;
            updateLockDisplay();
        } else if (tiltX < -0.3) {
            combination[activeSlot] = (combination[activeSlot] + 1) % 10;
            tiltCooldown = 0;
            updateLockDisplay();
        }
    }

    if (M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        for (auto& c : status.word) {
            if (c == ',') {
                if (activeSlot > 0) activeSlot--;
                M5Cardputer.Speaker.tone(1000, 30);
                updateLockDisplay();
                delay(150);
            } else if (c == '/') {
                if (activeSlot < 2) activeSlot++;
                M5Cardputer.Speaker.tone(1000, 30);
                updateLockDisplay();
                delay(150);
            }
        }

        if (status.enter) {
            M5Cardputer.Speaker.tone(1500, 50);
            if (combination[0] == secretCode[0] &&
                combination[1] == secretCode[1] &&
                combination[2] == secretCode[2]) {
                currentScene = SCENE_WIN;
                d.fillScreen(DARKGREEN);
                d.setTextSize(2);
                d.setCursor(30, 45);
                d.setTextColor(WHITE);
                d.print("YOU ESCAPED!");
                d.setTextSize(1);
                d.setCursor(20, 75);
                d.print("Happy Trick or Treating!");
                M5Cardputer.Speaker.tone(1200, 150);
                delay(160);
                M5Cardputer.Speaker.tone(1800, 250);
                while(1);
            } else {
                d.fillRect(20, 100, 200, 15, BLACK);
                d.setCursor(30, 100);
                d.setTextColor(RED);
                d.print("WRONG COMBINATION");
                delay(1200);
                drawLockScreen();
            }
        }
    }
}

void handleRoomInput() {
    if (M5Cardputer.Keyboard.isPressed()) {
        Keyboard_Class::KeysState status = M5Cardputer.Keyboard.keysState();
        for (auto& c : status.word) {
            if (c == ',' && selectedItem > 0) {
                selectedItem--;
                M5Cardputer.Speaker.tone(800, 20);
                updateRoomDisplay();
                delay(150);
            } else if (c == '/' && selectedItem < totalItems - 1) {
                selectedItem++;
                M5Cardputer.Speaker.tone(800, 20);
                updateRoomDisplay();
                delay(150);
            } else if (c == 'h' || c == 'H') {
                M5Cardputer.Speaker.tone(400, 100);
                d.fillRect(9, 102, 222, 26, BLACK);
                d.setCursor(12, 110);
                d.setTextColor(ORANGE);
                if (selectedItem == 0)      d.print("HINT: CLOCK VALUE IS 1");
                else if (selectedItem == 1) d.print("HINT: PAINTING VALUE IS 8");
                else if (selectedItem == 2) d.print("HINT: SAFE VALUE IS 8");
                else if (selectedItem == 3) d.print("HINT: 4/4 TIME -> DISTRACTOR");
                else if (selectedItem == 4) d.print("HINT: CHECK BOOK SPINES FOR ORDER");
            }
        }

        if (status.enter) {
            if (selectedItem == 3) {
                M5Cardputer.Speaker.tone(523, 100);
                delay(110);
                M5Cardputer.Speaker.tone(493, 100);
                delay(110);
                M5Cardputer.Speaker.tone(440, 100);
                musicBoxBeats = (musicBoxBeats % 4) + 1;
                drawItemGraphic(selectedItem);
            } else {
                M5Cardputer.Speaker.tone(2000, 40);
            }
        }
    }
}