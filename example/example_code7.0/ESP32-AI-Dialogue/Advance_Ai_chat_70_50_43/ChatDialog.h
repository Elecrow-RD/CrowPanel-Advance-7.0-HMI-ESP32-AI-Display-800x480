#ifndef CHATDIALOG_H
#define CHATDIALOG_H

#include <Arduino.h>                    // Arduino core library
#include <Wire.h>                       // I2C communication library
#include <TCA9534.h>                    // I/O expander chip driver
#include "LovyanGFX_Driver.h"           // Custom display driver

#define MAX_MESSAGES 20                 // Maximum number of messages (20)

class ChatDialog {
public:
    ChatDialog();                                                   // Constructor
    void begin();                                                   // Initialization method
    void update();                                                  // Update display
    void addMessage(String text, bool isAI);    // Add new message
    void appendToLastMessage(String text);                          // Append content to last message
    void showMicOrSpk(int status);                                  // Show microphone/speaker status

private:
    struct Message {
        String text;        // Message content
        bool isAI;          // Whether message is from AI
        int y;              // Vertical position
        int height;         // Height
        int currentLength;  // Current display length (for animation)
    };

    TCA9534 ioex;                   // I/O expander chip object
    LGFX tft;                       // Display driver object
    Message messages[MAX_MESSAGES]; // Message array

    int currentMicSpkStatus = 0;    // Current microphone/speaker status

    int messageCount = 0;                   // Current message count
    int scrollPosition = 0;                 // Scroll position
    int totalContentHeight = 0;             // Total content height
    unsigned long lastCharUpdate = 0;       // Last character update time
    const int CHAR_UPDATE_INTERVAL = 50;    // Character update interval (ms)

    void drawStaticElements();                                                      // Draw static UI elements
    void drawSingleMessage(int index);                                              // Draw single message
    void recalculateMessageLayout();                                                // Recalculate message layout
    void drawAllMessages();                                                         // Draw all messages
    void drawWrappedText(String text, int x, int y, int maxWidth, uint16_t color);  // Draw word-wrapped text
    int calculateTextHeight(String text, int maxWidth);                             // Calculate text height
    int findNextWordEnd(const String &text, int startPos);                          // Find next word end position
    
    // UI layout constants
    static const int DIALOG_WIDTH = 780;         // Dialog width
    static const int DIALOG_HEIGHT = 390;        // Dialog height
    static const int DIALOG_MARGIN_TOP = 70;     // Top margin
    static const int BUBBLE_RADIUS = 12;         // Bubble corner radius
    static const int BUBBLE_MARGIN = 20;         // Bubble spacing
    static const int TEXT_MARGIN = 12;           // Text margin
    static const int AVATAR_SIZE = 50;           // Avatar size
    static const int AVATAR_MARGIN = 15;         // Avatar margin
    static const int BUBBLE_CONTENT_WIDTH = 340; // Bubble content width
    static const int TEXT_SIZE = 1;              // Text size
    static const int CHAR_WIDTH = 6;             // Character width
    static const int LINE_HEIGHT = 18;           // Line height
    static const int DIALOG_INNER_PADDING = 5;   // Inner padding
    static const int ARROW_WIDTH = 12;           // Arrow width
};

#endif