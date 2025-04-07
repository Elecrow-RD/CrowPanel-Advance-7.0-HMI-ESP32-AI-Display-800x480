#include "ChatDialog.h"
#include "img/elecrow_logo.h"
#include "img/ai_logo.h"
#include "img/user_logo.h"
#include "img/mic_logo.h"
#include "img/spk_logo.h"

ChatDialog::ChatDialog() {}

void ChatDialog::showMicOrSpk(int status) {
  currentMicSpkStatus = status;
  int x = tft.width() / 2 - 40;
  int y = 20;
  int w = 40;
  int h = 40;

  if (status == 1) {
    tft.pushImage(x, y, w, h, MIC_LOGO);
  } else if (status == 2) {
    tft.pushImage(x, y, w, h, SPK_LOGO);
  } else {
    tft.fillRect(x, y, w, h, TFT_WHITE);
  }
}

void ChatDialog::begin() {
  Wire.begin(15, 16);
  delay(50);

  ioex.attach(Wire);
  ioex.setDeviceAddress(0x18);
  ioex.config(1, TCA9534::Config::OUT);
  ioex.config(2, TCA9534::Config::OUT);
  ioex.config(3, TCA9534::Config::OUT);
  ioex.config(4, TCA9534::Config::OUT);
  ioex.output(1, TCA9534::Level::H);

  tft.init();
  tft.setSwapBytes(true);
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);
  tft.setTextFont(0);

  drawStaticElements();
}

void ChatDialog::addMessage(String text, bool isAI) {
  Serial.println("接收到的数据:");
  Serial.println(text);

  // 1. Check if message exceeds maximum capacity
  if (messageCount >= MAX_MESSAGES) {                 // When exceeding capacity
      for (int i = 0; i < MAX_MESSAGES - 1; i++) {
          messages[i] = messages[i + 1];              // Shift messages forward (discard oldest)
      }
      messageCount = MAX_MESSAGES - 1;                // Maintain maximum capacity
  }

  // 2. Add new message to end of array
  messages[messageCount].text = text;
  messages[messageCount].isAI = isAI;

  // 3. Set initial display length: AI messages show character by character, user messages show immediately
  messages[messageCount].currentLength = isAI ? 0 : text.length();    // AI messages start with 0 characters
  messageCount++;     // Increment message count

  // 4. Recalculate layout for all messages (positions and heights)
  recalculateMessageLayout();

  // 5. Auto-scroll to bottom (calculate scroll position)
  scrollPosition = max(0, totalContentHeight - (DIALOG_HEIGHT - DIALOG_INNER_PADDING*2));

  // 6. Redraw all messages
  drawAllMessages();
}

/*
Purpose: Append content to last message (must be AI message) for streaming output
Parameters:
text: Text to append
*/
void ChatDialog::appendToLastMessage(String text) {
  addMessage(text, true);
}

/*
Purpose: Update AI message display character by character (typewriter effect)
*/
void ChatDialog::update() {
  if (millis() - lastCharUpdate > CHAR_UPDATE_INTERVAL) {   // Check every 50ms
    lastCharUpdate = millis();

    for (int i = 0; i < messageCount; i++) {
      if (messages[i].isAI && messages[i].currentLength < messages[i].text.length()) {
        int nextEnd = findNextWordEnd(messages[i].text, messages[i].currentLength);
        if (nextEnd > messages[i].currentLength) {      // More words to display
          messages[i].currentLength = nextEnd;
        } else {
          messages[i].currentLength = messages[i].text.length();
        }
        drawSingleMessage(i);                       // Partial redraw
      }
    }
  }
}

/*
Purpose: Find next word end position starting from startPos (for segmented display)
Parameters:
text: String to search
startPos: Starting search position
*/
int ChatDialog::findNextWordEnd(const String &text, int startPos) {
  int len = text.length();
  int endPos = startPos;

  while (endPos < len && text[endPos] == ' ') endPos++; // Skip leading spaces
  while (endPos < len && text[endPos] != ' ') endPos++; // Find word end
  while (endPos < len && text[endPos] == ' ') endPos++; // Skip trailing spaces

  return endPos;
}

/*
Purpose: Draw static UI elements (background, dialog border, title, Logo)
*/
void ChatDialog::drawStaticElements() {
  tft.fillScreen(TFT_WHITE);                              // White background
  // Draw rounded rectangle for dialog
  tft.drawRoundRect(tft.width() / 2 - DIALOG_WIDTH / 2,
                    DIALOG_MARGIN_TOP,
                    DIALOG_WIDTH,
                    DIALOG_HEIGHT,
                    20,
                    TFT_DARKGREY);

  // Draw "DEEPSEEK" title
  tft.setTextSize(3);
  tft.setTextColor(TFT_BLUE, TFT_WHITE);      // Blue text, white background
  tft.setCursor(30, 30);
  tft.print("DEEPSEEK");
  tft.pushImage(tft.width() - 180, 20, 150, 31, ELECROW_LOGO);    // Draw Elecrow company Logo

  // Display current status icon
  if (currentMicSpkStatus == 1) {
    tft.pushImage(tft.width() / 2 - 40, 20, 40, 40, MIC_LOGO);
  } else if (currentMicSpkStatus == 2) {
    tft.pushImage(tft.width() / 2 - 40, 20, 40, 40, SPK_LOGO);
  }
}

/*
Purpose: Recalculate layout for all messages (positions, heights)
*/
void ChatDialog::recalculateMessageLayout() {
  totalContentHeight = 0;                                                 // Reset total height
  int currentY = DIALOG_INNER_PADDING;                                    // Starting Y coordinate

  for (int i = 0; i < messageCount; i++) {
    int textWidth = BUBBLE_CONTENT_WIDTH - TEXT_MARGIN * 2;
    int textHeight = calculateTextHeight(messages[i].text, textWidth);  // Calculate text height
    int bubbleHeight = textHeight + TEXT_MARGIN * 2;                    // Bubble height = text height + top/bottom margins
    messages[i].height = max(bubbleHeight, AVATAR_SIZE);                // Take larger of bubble height and avatar size (for alignment)
    messages[i].y = currentY;                                    // Record Y position
    currentY += messages[i].height + BUBBLE_MARGIN;             // Accumulate height
    totalContentHeight = currentY + DIALOG_INNER_PADDING;       // Update total height
  }
}

/*
Purpose: Draw all visible messages (considering scroll position)
*/
void ChatDialog::drawAllMessages() {
  drawStaticElements();               // Draw background first

  // Set clipping region (only dialog interior)
    tft.setClipRect(tft.width()/2 - DIALOG_WIDTH/2,             // Start X
                   DIALOG_MARGIN_TOP + DIALOG_INNER_PADDING,    // Start Y
                   DIALOG_WIDTH,                                // Width
                   DIALOG_HEIGHT - DIALOG_INNER_PADDING*2);     // Height

  for (int i = 0; i < messageCount; i++) {
    int bubbleY = DIALOG_MARGIN_TOP + messages[i].y - scrollPosition;
    int messageBottom = bubbleY + messages[i].height;
    // Only draw messages in visible area
    if (messageBottom > (DIALOG_MARGIN_TOP + DIALOG_INNER_PADDING) && bubbleY < (DIALOG_MARGIN_TOP + DIALOG_HEIGHT - DIALOG_INNER_PADDING)) {
      drawSingleMessage(i);   // Draw single message
    }
  }
  tft.clearClipRect();    // Clear clipping region
}

/*
Purpose: Draw single message (including avatar, bubble, arrow, text)
Parameters:
index: Message index in messages array
*/
void ChatDialog::drawSingleMessage(int index) {
  Message msg = messages[index];
  int bubbleContentWidth = BUBBLE_CONTENT_WIDTH;
  int bubbleWidth = bubbleContentWidth + ARROW_WIDTH;
  int dialogX = tft.width() / 2 - DIALOG_WIDTH / 2;                       // Dialog left boundary

  int avatarX = msg.isAI ? (dialogX + AVATAR_MARGIN)                             // AI avatar on left
                         : (dialogX + DIALOG_WIDTH - AVATAR_SIZE - AVATAR_MARGIN);  // User avatar on right
  int baseY = DIALOG_MARGIN_TOP + DIALOG_INNER_PADDING + msg.y - scrollPosition;

  int bubbleX, arrowX, arrowY;        // Calculate bubble position
  if (msg.isAI) {
    bubbleX = avatarX + AVATAR_SIZE + AVATAR_MARGIN - ARROW_WIDTH;
    arrowX = bubbleX + ARROW_WIDTH;
    arrowY = baseY + AVATAR_SIZE / 2 - ARROW_WIDTH / 2;
  } else {
    bubbleX = avatarX - bubbleWidth;
    arrowX = bubbleX + bubbleContentWidth;
    arrowY = baseY + AVATAR_SIZE / 2 - ARROW_WIDTH / 2;
  }
  // Draw avatar
  tft.pushImage(avatarX, baseY, AVATAR_SIZE, AVATAR_SIZE, msg.isAI ? AI_LOGO : USER_LOGO);
  // Draw bubble body
  tft.fillRoundRect(
    msg.isAI ? (bubbleX + ARROW_WIDTH) : bubbleX,       // AI bubble shifted by arrow width
    baseY,
    bubbleContentWidth,
    msg.height,
    BUBBLE_RADIUS,
    msg.isAI ? 0xE71C : 0xBDF7);          // AI uses orange-red, user uses light blue
  // Draw bubble arrow
  if (msg.isAI) {      // AI arrow points left
    tft.fillTriangle(
      bubbleX + ARROW_WIDTH, arrowY,
      bubbleX + ARROW_WIDTH, arrowY + ARROW_WIDTH,
      bubbleX, arrowY + ARROW_WIDTH / 2,
      0xE71C);
  } else {           // User arrow points right
    tft.fillTriangle(
      arrowX, arrowY,
      arrowX, arrowY + ARROW_WIDTH,
      arrowX + ARROW_WIDTH, arrowY + ARROW_WIDTH / 2,
      0xBDF7);
  }

  // Draw text (truncated by currentLength)
  String displayText = msg.text.substring(0, msg.currentLength);
  drawWrappedText(
    displayText,
    msg.isAI ? (bubbleX + ARROW_WIDTH + TEXT_MARGIN) : (bubbleX + TEXT_MARGIN),
    baseY + TEXT_MARGIN,
    BUBBLE_CONTENT_WIDTH - TEXT_MARGIN * 2,
    msg.isAI ? TFT_BLACK : TFT_NAVY);     // AI black text, user dark blue
}

/*
Purpose: Calculate total text height within specified width (lines × line height)
Parameters:
text: Text to calculate
maxWidth: Maximum allowed width (pixels)
*/
int ChatDialog::calculateTextHeight(String text, int maxWidth) {
  tft.setTextSize(TEXT_SIZE);             // Set text size
  int charWidth = CHAR_WIDTH * TEXT_SIZE; // Single character width
  int spaceWidth = charWidth;             // Space width
  int lines = 1;                          // Total lines
  int currentLineWidth = 0;               // Current line width used
  int currentWordWidth = 0;               // Current word width

  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];

    if (c == ' ' || c == '\n') {        // Encounter space or newline
      if (currentWordWidth > 0) {     // Process current word
        if (currentLineWidth + currentWordWidth > maxWidth) {
          lines++;
          currentLineWidth = currentWordWidth;
        } else {
          currentLineWidth += currentWordWidth;
        }
        currentWordWidth = 0;
      }

      if (c == '\n') {                // Force newline
        lines++;
        currentLineWidth = 0;
      } else if (currentLineWidth + spaceWidth > maxWidth) {  // Space causes newline
        lines++;
        currentLineWidth = spaceWidth;
      } else {
        currentLineWidth += spaceWidth;
      }
    } else {                            // Normal character
      currentWordWidth += charWidth;
      if (currentWordWidth > maxWidth) {      // Word too long, split
        lines++;
        currentLineWidth = currentWordWidth - charWidth;
        currentWordWidth = charWidth;
      }
    }
  }

  if (currentWordWidth > 0) {             // Process last word
    if (currentLineWidth + currentWordWidth > maxWidth) lines++;
  }

  return lines * LINE_HEIGHT;     // Total lines × line height
}

/*
Purpose: Draw word-wrapped text at specified position
Parameters:
text: Text to draw
x, y: Starting coordinates (top-left)
maxWidth: Maximum single line width
color: Text color (RGB565 format)
*/
void ChatDialog::drawWrappedText(String text, int x, int y, int maxWidth, uint16_t color) {
  tft.setTextColor(color);
  tft.setTextSize(TEXT_SIZE);

  int charWidth = CHAR_WIDTH * TEXT_SIZE;
  int spaceWidth = charWidth;
  int cursorX = x;            // Current draw X coordinate
  int cursorY = y;            // Current draw Y coordinate
  int currentLineWidth = 0;   // Current line width used
  String currentWord = "";    // Current word buffer
  int currentWordWidth = 0;   // Current word width

  for (unsigned int i = 0; i < text.length(); i++) {
    char c = text[i];

  if (c == ' ' || c == '\n') {            // Encounter separator
    if (currentWord.length() > 0) {     // Draw buffered word
        if (currentLineWidth + currentWordWidth > maxWidth) {   // Need newline
          cursorY += LINE_HEIGHT;
          cursorX = x;
          currentLineWidth = 0;
        }
        tft.setCursor(cursorX, cursorY);
        tft.print(currentWord);         // Draw word
        cursorX += currentWordWidth;
        currentLineWidth += currentWordWidth;
        currentWord = "";
        currentWordWidth = 0;
      }

      if (c == '\n') {                // Draw word
        cursorY += LINE_HEIGHT;
        cursorX = x;
        currentLineWidth = 0;
      } else {                        // Process space
        if (currentLineWidth + spaceWidth > maxWidth) {
          cursorY += LINE_HEIGHT;
          cursorX = x;
          currentLineWidth = 0;
        }
        tft.drawChar(' ', cursorX, cursorY);    // Draw space
        cursorX += spaceWidth;
        currentLineWidth += spaceWidth;
      }
    } else {               // Normal character
      currentWord += c;
      currentWordWidth += charWidth;

      if (currentWordWidth > maxWidth) {          // Word too long needs splitting
        int availableWidth = maxWidth - currentLineWidth;
        int charsToDraw = availableWidth / charWidth;       // Drawable characters

        if (charsToDraw > 0) {                // Draw leading characters
          tft.setCursor(cursorX, cursorY);
          tft.print(currentWord.substring(0, charsToDraw));
          cursorX += charsToDraw * charWidth;
          currentLineWidth += charsToDraw * charWidth;
        }

        cursorY += LINE_HEIGHT;
        cursorX = x;
        currentWord = currentWord.substring(charsToDraw);
        currentWordWidth = currentWord.length() * charWidth;
        currentLineWidth = 0;
      }
    }
  }

  if (currentWord.length() > 0) {
    if (currentLineWidth + currentWordWidth > maxWidth) {
      cursorY += LINE_HEIGHT;
      cursorX = x;
    }
    tft.setCursor(cursorX, cursorY);
    tft.print(currentWord);
  }
}