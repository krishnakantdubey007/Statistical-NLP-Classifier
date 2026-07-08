// =============================================================================
// Statistical NLP Text Classifier Engine  -  GUI  (gui_main.cpp)
// Multi-page desktop application built with Raylib + RayGUI
//
// Pages:
//   0 = Main Menu
//   1 = Predict Message
//   2 = Model Statistics
//   3 = About Project
//   4 = Exit Confirmation
// =============================================================================

#include <string>
#include <vector>
#include <cstring>
#include <cstdio>
#include <cmath>

#include "DataLoader.hpp"
#include "NaiveBayes.hpp"
#include <raylib.h>
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "Theme.hpp"

// =============================================================================
//  Window dimensions
// =============================================================================

static const int SW = 1120;
static const int SH = 700;

// =============================================================================
//  Extended colour palette
//  (Theme:: colours remain the canonical source; these extend it locally)
// =============================================================================

// Accent colours
static const Color C_ACCENT      = {  82, 130, 255, 255 };  // bright indigo-blue
static const Color C_ACCENT_DIM  = {  55,  90, 180, 255 };  // muted accent for pressed state

static const Color C_DANGER      = { 235,  75,  75, 255 };  // vivid red
static const Color C_DANGER_SOFT = { 235,  75,  75,  40 };  // red tint bg
static const Color C_SUCCESS     = {  46, 210, 132, 255 };  // bright mint
static const Color C_SUCCESS_SOFT= {  46, 210, 132,  40 };  // mint tint bg

// Surface hierarchy
static const Color C_BG          = {  18,  20,  26, 255 };  // deep canvas
static const Color C_SURFACE_1   = {  26,  29,  38, 255 };  // card bg
static const Color C_SURFACE_2   = {  34,  38,  50, 255 };  // inner surface
static const Color C_SURFACE_3   = {  44,  50,  66, 255 };  // hover / active

// Text hierarchy
static const Color C_TEXT_H      = { 240, 242, 248, 255 };  // headline
static const Color C_TEXT_B      = { 178, 185, 200, 255 };  // body
static const Color C_TEXT_M      = { 110, 120, 145, 255 };  // muted / label

// Border
static const Color C_BORDER      = {  52,  58,  78, 255 };  // subtle edge
static const Color C_BORDER_LIT  = {  82, 130, 255,  90 };  // lit edge on focus

// Header gradient layers (painted row-by-row to simulate a gradient)
static const Color C_HEADER_TOP  = {  22,  26,  36, 255 };
static const Color C_HEADER_BOT  = {  18,  20,  28, 255 };

// =============================================================================
//  Design token helpers
// =============================================================================

// Sets the alpha channel of a colour without modifying RGB.
static Color WithAlpha(Color c, unsigned char a)
{
    c.a = a;
    return c;
}

// BUG-04 FIX: Original version did unsigned arithmetic on unsigned char, causing
// wraparound when b < a (e.g. C_HEADER_BOT.r=18 - C_HEADER_TOP.r=22 wrapped to 252).
// Fixed by widening to int before subtraction so the difference is signed.
static Color LerpColor(Color a, Color b, float t)
{
    return Color{
        (unsigned char)((int)a.r + (int)(((int)b.r - (int)a.r) * t)),
        (unsigned char)((int)a.g + (int)(((int)b.g - (int)a.g) * t)),
        (unsigned char)((int)a.b + (int)(((int)b.b - (int)a.b) * t)),
        255
    };
}

// =============================================================================
//  Primitive drawing helpers
// =============================================================================

// Card: filled rounded rect + subtle drop-shadow + border
static void DrawCard(float x, float y, float w, float h, float radius = 0.07f)
{
    DrawRectangleRounded({x + 3, y + 5, w, h}, radius, 12, {0, 0, 0, 55});
    DrawRectangleRounded({x, y, w, h}, radius, 12, C_SURFACE_1);
    DrawRectangleRoundedLines({x, y, w, h}, radius, 12, C_BORDER);
}

// Card with a slim accent stripe along the top edge
static void DrawCardAccented(float x, float y, float w, float h)
{
    DrawCard(x, y, w, h);
    DrawRectangleRounded({x + 2, y, w - 4, 3}, 1.0f, 4, C_ACCENT);
}

// Horizontal rule
static void DrawRule(float x, float y, float w)
{
    DrawLineEx({x, y}, {x + w, y}, 1.0f, C_BORDER);
}
// =============================================================================
//  Custom Font Setup & Shadow Functions
// =============================================================================

static Font g_font;

#define DrawText(text, x, y, size, col) DrawTextEx(g_font, (text), {(float)(x), (float)(y)}, (float)(size), 1.0f, (col))
#define MeasureText(text, size) ((int)MeasureTextEx(g_font, (text), (float)(size), 1.0f).x)

// Centred text
static void DrawTC(const char* text, float cx, float y, int size, Color col)
{
    int tw = MeasureText(text, size);
    DrawText(text, (int)(cx - tw * 0.5f), (int)y, size, col);
}

// Right-aligned text
static void DrawTR(const char* text, float rightEdge, float y, int size, Color col)
{
    int tw = MeasureText(text, size);
    DrawText(text, (int)(rightEdge - tw), (int)y, size, col);
}

// BUG-07 (UI-07) FIX: Pill badge with adaptive vertical text centering.
// Original hard-coded "+ 6" only centred correctly at one specific font size.
// Fixed by computing the offset as (pill_height - font_size) / 2.
static void DrawPill(const char* text, float x, float y, int fontSize,
                     Color bg, Color fg)
{
    int   tw = MeasureText(text, fontSize);
    float pw = (float)(tw + 22);
    float ph = (float)(fontSize + 12);
    DrawRectangleRounded({x, y, pw, ph}, 1.0f, 8, bg);
    // Vertically centre the text within the pill
    float textY = y + (ph - (float)fontSize) * 0.5f;
    DrawText(text, (int)(x + 11), (int)textY, fontSize, fg);
}

// BUG-03 FIX: Original cast cx/cy to int, causing sub-pixel snapping that
// makes circles drawn at fractional positions appear mis-centred.
// DrawCircleV accepts float centre coordinates directly.
static void DrawCircleFilled(float cx, float cy, float r, Color col)
{
    DrawCircleV({cx, cy}, r, col);
}

// NOTE: DrawRatioBar was removed (BUG-05). It was defined but never called;
// the Statistics page draws the ratio bar inline, so this was dead code.

// =============================================================================
//  Composite UI components
// =============================================================================

// UI-06 FIX: DrawSubHeader now draws a styled nav bar (y=82 to y=130) directly
// under the main header, with top and bottom border lines.  Previously the only
// visual element was a floating separator at y=130 with no background, leaving
// a 48px unstyled dead zone that broke the visual rhythm.
static bool DrawSubHeader(const char* title, int screenW)
{
    // Styled secondary nav bar directly below the main header
    DrawRectangle(0, 82, screenW, 48, C_SURFACE_1);
    DrawRectangle(0, 82,  screenW, 1, C_BORDER);   // top edge (tight to header)
    DrawRectangle(0, 130, screenW, 1, C_BORDER);   // bottom edge

    GuiSetStyle(DEFAULT, TEXT_SIZE, 17);
    bool back = GuiButton({16, 88, 100, 34}, "< Back");

    DrawText(title, 130, 93, 20, C_ACCENT);
    return back;
}

// BUG-06 FIX: DrawStatTile previously computed vw (value text width) but then
// suppressed the warning with (void)vw and drew the value left-aligned at x+62,
// causing long strings like "Naive Bayes" to overflow the tile's right edge.
// Fixed by using vw to horizontally centre the value within the tile width.
static void DrawStatTile(float x, float y, float w, float h,
                         const char* label, const char* value,
                         Color iconBg, Color valCol)
{
    DrawCard(x, y, w, h);
    DrawRectangleRounded({x + 2, y, w - 4, 3}, 1.0f, 4, iconBg);  // accent strip

    // Icon circles
    DrawCircleFilled(x + 36, y + h * 0.5f, 18, WithAlpha(iconBg, 35));
    DrawCircleFilled(x + 36, y + h * 0.5f, 12, iconBg);

    // Label (small, right of icon)
    DrawText(label, (int)(x + 62), (int)(y + 14), 14, C_TEXT_M);

    // Value: measure then centre within the tile (BUG-06 core fix)
    int vw = MeasureText(value, 28);
    DrawText(value, (int)(x + w * 0.5f - vw * 0.5f), (int)(y + 40), 28, valCol);
}

// Info row inside the About card: dot indicator + label + value
static void DrawInfoRow(float cardX, float y, float valueX,
                        const char* label, const char* value, Color valCol)
{
    DrawCircleFilled(cardX + 22, y + 9, 4, C_ACCENT);
    DrawText(label, (int)(cardX + 34), (int)y, 17, C_TEXT_M);
    DrawText(value, (int)valueX,       (int)y, 17, valCol);
}

// =============================================================================
//  BUG-01 FIX: Error screen drawn inside the Raylib window when the dataset
//  cannot be loaded.  Previously loadDataset threw an unhandled exception that
//  crashed the process before any window opened.
//  Returns true when the user clicks "Quit".
// =============================================================================

static bool DrawLoadError(const char* errorMsg, int screenW, int screenH)
{
    float dW = 580, dH = 250;
    float dX = (screenW - dW) * 0.5f;
    float dY = (screenH - dH) * 0.5f;

    // Glow halo
    DrawRectangleRounded({dX - 8, dY - 8, dW + 16, dH + 16},
                         0.1f, 12, WithAlpha(C_DANGER, 18));
    DrawCardAccented(dX, dY, dW, dH);

    // Warning icon
    float icCX = dX + dW * 0.5f;
    DrawCircleFilled(icCX, dY + 52, 26, WithAlpha(C_DANGER, 30));
    DrawCircleFilled(icCX, dY + 52, 20, C_DANGER);
    DrawTC("!", icCX, dY + 34, 26, WHITE);

    DrawTC("Dataset Load Error", icCX, dY + 86, 22, C_TEXT_H);

    // R-BUG-02 FIX: errorMsg is the raw ex.what() string which can be long
    // enough to overflow the 580px dialog card.  Truncate to 60 characters
    // with a trailing ellipsis so it always fits within the card width.
    char truncMsg[64];
    if (strlen(errorMsg) > 60)
    {
        strncpy(truncMsg, errorMsg, 57);
        truncMsg[57] = '.';
        truncMsg[58] = '.';
        truncMsg[59] = '.';
        truncMsg[60] = '\0';
    }
    else
    {
        strncpy(truncMsg, errorMsg, sizeof(truncMsg) - 1);
        truncMsg[sizeof(truncMsg) - 1] = '\0';
    }
    DrawTC(truncMsg, icCX, dY + 118, 14, C_DANGER);
    DrawTC("Ensure  data/SMSSpamCollection  is at the project root, then rebuild.",
           icCX, dY + 142, 13, C_TEXT_M);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    float btnW = 130, btnH = 40;
    if (GuiButton({dX + (dW - btnW) * 0.5f, dY + dH - 56, btnW, btnH}, "Quit"))
        return true;

    return false;
}

// =============================================================================
//  Header  (shared on every page)
// =============================================================================

static void DrawHeader(int screenW)
{
    // Row-by-row gradient (LerpColor now uses signed int arithmetic — BUG-04)
    for (int i = 0; i < 80; ++i)
    {
        float t   = (float)i / 79.0f;
        Color row = LerpColor(C_HEADER_TOP, C_HEADER_BOT, t);
        DrawRectangle(0, i, screenW, 1, row);
    }

    DrawRectangle(0, 79, screenW, 1, C_BORDER);
    DrawRectangle(0, 80, screenW, 2, WithAlpha(C_ACCENT, 60));  // accent glow strip

    // Icon badge
    DrawRectangleRounded({18, 14, 46, 46}, 0.35f, 8, C_ACCENT);
    DrawRectangleRoundedLines({18, 14, 46, 46}, 0.35f, 8, WithAlpha(C_ACCENT, 120));
    int nlpW = MeasureText("NLP", 16);
    DrawText("NLP", 18 + 23 - nlpW / 2, 14 + 15, 16, WHITE);

    DrawText("Statistical NLP Text Classifier Engine",         80, 15, 26, C_TEXT_H);
    DrawText("Multinomial Naive Bayes  |  Spam Detection Engine", 80, 47, 16, C_TEXT_M);

    const char* dev = "Krishnakant Dubey";
    int devW = MeasureText(dev, 15);
    DrawText(dev, screenW - devW - 22, 34, 15, C_TEXT_M);
}

// =============================================================================
//  Footer  (shared on inner pages)
// =============================================================================

static void DrawFooter(int screenW, int screenH)
{
    DrawRectangle(0, screenH - 34, screenW, 34, C_SURFACE_1);
    DrawRectangle(0, screenH - 34, screenW, 1, C_BORDER);
    DrawTC("Statistical NLP Classifier Engine  v2.0   |   Raylib + RayGUI   |   Krishnakant Dubey",
           screenW * 0.5f, screenH - 22, 13, C_TEXT_M);
}

// =============================================================================
//  Page 0  -  Main Menu
// =============================================================================

static void DrawMainMenu(int& currentPage, int screenW, int screenH)
{
    // Subtle background grid (decorative)
    for (int gx = 0; gx < screenW; gx += 60)
        DrawLineEx({(float)gx, 82}, {(float)gx, (float)screenH}, 1.0f, {255, 255, 255, 5});
    for (int gy = 82; gy < screenH; gy += 60)
        DrawLineEx({0, (float)gy}, {(float)screenW, (float)gy}, 1.0f, {255, 255, 255, 5});

    // Hero card
    float cW = 480, cH = 430;
    float cX = (screenW - cW) * 0.5f;
    float cY = (screenH - cH) * 0.5f - 10;

    DrawRectangleRounded({cX - 6, cY - 6, cW + 12, cH + 12},
                         0.09f, 12, WithAlpha(C_ACCENT, 22));
    DrawCardAccented(cX, cY, cW, cH);

    float iconCX = cX + cW * 0.5f;
    DrawCircleFilled(iconCX, cY + 60, 36, WithAlpha(C_ACCENT, 30));
    DrawCircleFilled(iconCX, cY + 60, 26, C_ACCENT);
    DrawTC("NLP", iconCX, cY + 47, 18, WHITE);

    DrawTC("Statistical NLP Classifier",               iconCX, cY + 102, 22, C_TEXT_H);
    DrawTC("Multinomial Naive Bayes  |  Spam Detection", iconCX, cY + 130, 14, C_TEXT_M);

    float pillW = (float)(MeasureText("v 2.0", 14) + 22);
    DrawPill("v 2.0", iconCX - pillW * 0.5f, cY + 156, 14, C_SURFACE_3, C_ACCENT);

    DrawRule(cX + 32, cY + 188, cW - 64);

    float btnW = 300, btnH = 48;
    float btnX = cX + (cW - btnW) * 0.5f;
    float bY0  = cY + 204;
    float bGap = 58;

    GuiSetStyle(DEFAULT, TEXT_SIZE, 19);

    if (GuiButton({btnX, bY0,          btnW, btnH}, "  Predict Message"))  currentPage = 1;
    if (GuiButton({btnX, bY0 + bGap,   btnW, btnH}, "  Model Statistics")) currentPage = 2;
    if (GuiButton({btnX, bY0 + bGap*2, btnW, btnH}, "  About Project"))    currentPage = 3;
    if (GuiButton({btnX, bY0 + bGap*3, btnW, btnH}, "  Exit"))             currentPage = 4;

    DrawTC("Developed by  Krishnakant Dubey", iconCX, cY + cH - 30, 14, C_TEXT_M);
}

// =============================================================================
//  Page 1  -  Predict Message
// =============================================================================

// CPP-01 FIX: Three former static file-scope globals replaced with a named
// struct so the predict-page state is cohesive, testable, and clearly owned
// by main().  The struct is passed by reference into DrawPredictPage.
struct PredictState
{
    char        message[512] = {};    // TextBox buffer
    bool        editMode     = false; // TextBox active?
    std::string prediction   = "---"; // last classifier result
};

static void DrawPredictPage(int& currentPage, NaiveBayes& classifier,
                            PredictState& ps, int screenW, int screenH)
{
    // LOGIC-03 FIX: reset editMode when leaving the page so the TextBox does
    // not re-activate the moment the user returns.
    if (DrawSubHeader("Predict Message", screenW))
    {
        ps.editMode = false;
        currentPage = 0;
        return;
    }

    // Input card
    float iX = 36, iY = 146, iW = screenW - 72.0f, iH = 168;
    DrawCardAccented(iX, iY, iW, iH);
    DrawText("Message Input", (int)iX + 22, (int)iY + 16, 16, C_TEXT_M);

    // TextBox
    Rectangle tb = {iX + 20, iY + 40, iW - 40, 96};
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        ps.editMode = CheckCollisionPointRec(GetMousePosition(), tb);

    if (ps.editMode)
        DrawRectangleRoundedLines({tb.x - 2, tb.y - 2, tb.width + 4, tb.height + 4},
                                  0.06f, 8, C_BORDER_LIT);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);
    GuiTextBox(tb, ps.message, 512, ps.editMode);

    // Character counter
    char cntBuf[32];
    snprintf(cntBuf, sizeof(cntBuf), "%d / 512", (int)strlen(ps.message));
    DrawTR(cntBuf, iX + iW - 22, iY + iH - 22, 13, C_TEXT_M);

    // Action buttons
    float bY = iY + iH + 18;
    float bH = 44;
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

    if (GuiButton({iX, bY, 180, bH}, "  Predict"))
    {
        // LOGIC-02: strip whitespace before deciding whether to classify.
        // strlen(ps.message) > 0 would pass for "   " (all spaces), producing
        // an empty token list and a wrong result from the classifier.
        std::string msg(ps.message);
        size_t start = msg.find_first_not_of(" \t\r\n");
        if (start != std::string::npos)
            ps.prediction = classifier.predict(msg);
    }

    if (GuiButton({iX + 196, bY, 130, bH}, "Clear"))
    {
        memset(ps.message, 0, sizeof(ps.message));
        ps.prediction = "---";
        ps.editMode   = false;
    }

    // Result card
    float rY = bY + bH + 16;
    float rH = screenH - rY - 38.0f;
    DrawCard(iX, rY, iW, rH);

    DrawText("Prediction Result", (int)iX + 22, (int)rY + 16, 16, C_TEXT_M);
    DrawRule(iX + 18, rY + 42, iW - 36);

    // Status indicator dot
    Color dotCol = C_TEXT_M;
    if      (ps.prediction == "spam") dotCol = C_DANGER;
    else if (ps.prediction == "ham")  dotCol = C_SUCCESS;
    DrawCircleFilled(iX + 30, rY + 76, 7, dotCol);

    // Prediction word + pill
    Color resCol = C_TEXT_M;
    if      (ps.prediction == "spam") resCol = C_DANGER;
    else if (ps.prediction == "ham")  resCol = C_SUCCESS;

    DrawText("Prediction",          (int)iX + 50, (int)(rY + 64), 15, C_TEXT_M);
    DrawText(ps.prediction.c_str(), (int)iX + 50, (int)(rY + 82), 26, resCol);

    if (ps.prediction == "spam")
        DrawPill("SPAM", iX + 230, rY + 83, 16, C_DANGER_SOFT, C_DANGER);
    else if (ps.prediction == "ham")
        DrawPill("HAM", iX + 230, rY + 83, 16, C_SUCCESS_SOFT, C_SUCCESS);

    // Confidence row
    DrawRule(iX + 18, rY + 120, iW - 36);
    DrawText("Confidence", (int)iX + 50, (int)(rY + 132), 15, C_TEXT_M);

    if (ps.prediction == "---" || ps.prediction == "unknown")
        DrawText("Awaiting input...", (int)iX + 50, (int)(rY + 150), 17, C_TEXT_M);
    else if (ps.prediction == "untrained")
        DrawText("Model not trained.", (int)iX + 50, (int)(rY + 150), 17, C_DANGER);
    else
        DrawText("High   (Laplace-smoothed log-probability score)",
                 (int)iX + 50, (int)(rY + 150), 17, C_SUCCESS);

    DrawFooter(screenW, screenH);
}

// =============================================================================
//  Page 2  -  Model Statistics
// =============================================================================

static void DrawStatisticsPage(int& currentPage, NaiveBayes& classifier,
                               const std::vector<Message>& dataset,
                               int screenW, int screenH)
{
    if (DrawSubHeader("Model Statistics", screenW))
    {
        currentPage = 0;
        return;
    }

    int spamCount  = classifier.getSpamMessageCount();
    int hamCount   = classifier.getHamMessageCount();
    int totalCount = spamCount + hamCount;
    int vocabSize  = classifier.getVocabularySize();

    // UI-02 FIX: tile width is computed from screenW so the three tiles always
    // fill the content area regardless of window width, with 36px side margins.
    // Previously tW was hard-coded to 330, which barely fit at SW=1120 and
    // would overflow at any narrower size.
    float tGap = 18.0f, tH = 102.0f;
    float tW   = ((float)screenW - 72.0f - 2.0f * tGap) / 3.0f;
    float totalTileW = 3.0f * tW + 2.0f * tGap;
    float tX0  = ((float)screenW - totalTileW) * 0.5f;
    float tY1  = 148.0f, tY2 = tY1 + tH + tGap;

    char buf[64];

    snprintf(buf, sizeof(buf), "%d", totalCount);
    DrawStatTile(tX0,              tY1, tW, tH, "Total Messages", buf, C_ACCENT, C_TEXT_H);

    snprintf(buf, sizeof(buf), "%d", spamCount);
    DrawStatTile(tX0 + tW + tGap,  tY1, tW, tH, "Spam Messages",  buf, C_DANGER, C_DANGER);

    snprintf(buf, sizeof(buf), "%d", hamCount);
    DrawStatTile(tX0 + 2*(tW+tGap),tY1, tW, tH, "Ham Messages",   buf, C_SUCCESS, C_SUCCESS);

    snprintf(buf, sizeof(buf), "%d", vocabSize);
    DrawStatTile(tX0,              tY2, tW, tH, "Vocabulary Size", buf, C_ACCENT, C_TEXT_H);

    DrawStatTile(tX0 + tW + tGap,  tY2, tW, tH, "Training Status", "Trained",     C_SUCCESS, C_SUCCESS);
    DrawStatTile(tX0 + 2*(tW+tGap),tY2, tW, tH, "Algorithm",       "Naive Bayes", C_ACCENT, C_TEXT_H);

    // Details card
    float dY = tY2 + tH + tGap;
    float dW = totalTileW;
    float dX = tX0;
    float dH = (float)screenH - dY - 38.0f;
    DrawCard(dX, dY, dW, dH);

    DrawText("Dataset Overview", (int)dX + 22, (int)dY + 14, 16, C_TEXT_M);
    DrawRule(dX + 16, dY + 40, dW - 32);

    // Spam / Ham ratio bar
    float barX = dX + 22, barY = dY + 52, barW = dW - 44, barH = 18;
    DrawRectangleRounded({barX, barY, barW, barH}, 0.5f, 8, C_SURFACE_3);

    if (totalCount > 0)
    {
        float spamRatio = (float)spamCount / (float)totalCount;
        float hamRatio  = (float)hamCount  / (float)totalCount;

        DrawRectangleRounded({barX,                    barY, barW * spamRatio, barH},
                             0.5f, 8, C_DANGER);
        DrawRectangleRounded({barX + barW * spamRatio, barY, barW * hamRatio,  barH},
                             0.5f, 8, C_SUCCESS);

        char pBuf[16];
        snprintf(pBuf, sizeof(pBuf), "%.0f%% Spam", spamRatio * 100.0f);
        DrawText(pBuf, (int)barX, (int)(barY + 24), 14, C_DANGER);

        snprintf(pBuf, sizeof(pBuf), "%.0f%% Ham", hamRatio * 100.0f);
        DrawTR(pBuf, barX + barW, barY + 24, 14, C_SUCCESS);
    }

    DrawText("Dataset  :  SMSSpamCollection  (UCI ML Repository, tab-separated)",
             (int)barX, (int)(barY + 48), 16, C_TEXT_B);

    DrawFooter(screenW, screenH);
}

// =============================================================================
//  Page 3  -  About Project
// =============================================================================

static void DrawAboutPage(int& currentPage, int screenW, int screenH)
{
    if (DrawSubHeader("About Project", screenW))
    {
        currentPage = 0;
        return;
    }

    float colGap = 18.0f;
    float leftW  = 640.0f;
    float rightW = (float)screenW - 72.0f - leftW - colGap;
    float cardY  = 146.0f;
    float cardH  = (float)screenH - cardY - 38.0f;
    float leftX  = 36.0f;
    float rightX = leftX + leftW + colGap;

    // Left: project info
    DrawCardAccented(leftX, cardY, leftW, cardH);
    DrawText("Project Information", (int)leftX + 22, (int)cardY + 14, 16, C_TEXT_M);
    DrawRule(leftX + 16, cardY + 40, leftW - 32);

    struct InfoRow { const char* label; const char* value; Color valCol; };
    InfoRow rows[] = {
        { "Project Name",  "Statistical NLP Text Classifier Engine", C_TEXT_H  },
        { "Developer",     "Krishnakant Dubey",                      C_ACCENT  },
        { "Language",      "C++17",                                   C_SUCCESS },
        { "Algorithm",     "Multinomial Naive Bayes (Laplace)",       C_TEXT_B  },
        { "Dataset",       "SMSSpamCollection (UCI ML Repository)",   C_TEXT_B  },
        { "GUI Framework", "Raylib 5.x  +  RayGUI 4.x",             C_TEXT_B  },
        { "Build System",  "CMake 3.20+",                             C_TEXT_B  },
        { "Version",       "2.0",                                     C_ACCENT  },
        { "GitHub",        "github.com/krishnakantdubey007",          C_ACCENT  },
    };

    int   rowCount  = (int)(sizeof(rows) / sizeof(rows[0]));
    float rowStartY = cardY + 54;
    float rowGap    = 40;
    float valX      = leftX + 220;

    for (int i = 0; i < rowCount; ++i)
    {
        float ry = rowStartY + i * rowGap;
        if (i % 2 == 0)
            DrawRectangle((int)(leftX + 4), (int)ry - 4,
                          (int)(leftW - 8), (int)rowGap - 4,
                          WithAlpha(C_SURFACE_2, 90));
        DrawInfoRow(leftX, ry, valX, rows[i].label, rows[i].value, rows[i].valCol);
    }

    
   

    // Right: tech stack
    DrawCardAccented(rightX, cardY, rightW, cardH);
    DrawText("Tech Stack", (int)rightX + 22, (int)cardY + 14, 16, C_TEXT_M);
    DrawRule(rightX + 16, cardY + 40, rightW - 32);

    struct TechRow { const char* abbr; const char* name; Color col; };
    TechRow techs[] = {
        { "C++",  "C++17 Standard",  C_SUCCESS },
        { "ML",   "Naive Bayes NLP", C_ACCENT  },
        { "DB",   "UCI SMS Dataset", C_TEXT_B  },
        { "GUI",  "Raylib + RayGUI", C_ACCENT  },
        { "BLD",  "CMake Build",     C_TEXT_B  },
        { "VER",  "Version 2.0",     C_ACCENT  },
    };

    int   techCount = (int)(sizeof(techs) / sizeof(techs[0]));
    float techY0    = cardY + 56;
    float techGap   = 52;

    for (int i = 0; i < techCount; ++i)
    {
        float ty = techY0 + i * techGap;
        DrawCircleFilled(rightX + 32, ty + 16, 18, WithAlpha(techs[i].col, 30));
        DrawCircleFilled(rightX + 32, ty + 16, 12, WithAlpha(techs[i].col, 120));
        int bw = MeasureText(techs[i].abbr, 11);
        DrawText(techs[i].abbr, (int)(rightX + 32 - bw * 0.5f), (int)(ty + 10), 11, WHITE);
        DrawText(techs[i].name, (int)(rightX + 58), (int)(ty + 8), 16, techs[i].col);
    }

    DrawFooter(screenW, screenH);
}

// =============================================================================
//  Page 4  -  Exit Confirmation
// =============================================================================

static bool DrawExitPage(int& currentPage, int screenW, int screenH)
{
    DrawRectangle(0, 0, screenW, screenH, {0, 0, 0, 175});

    float dW = 420, dH = 240;
    float dX = (screenW - dW) * 0.5f;
    float dY = (screenH - dH) * 0.5f;

    DrawRectangleRounded({dX - 8, dY - 8, dW + 16, dH + 16},
                         0.1f, 12, WithAlpha(C_DANGER, 18));
    DrawCardAccented(dX, dY, dW, dH);

    float icCX = dX + dW * 0.5f, icCY = dY + 60;
    DrawCircleFilled(icCX, icCY, 30, WithAlpha(C_DANGER, 25));
    DrawCircleFilled(icCX, icCY, 22, C_DANGER);
    DrawTC("!", icCX, icCY - 14, 28, WHITE);

    DrawTC("Exit Application?",            icCX, dY + 100, 22, C_TEXT_H);
    DrawTC("Are you sure you want to quit?", icCX, dY + 130, 15, C_TEXT_M);

    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

    float btnW = 142, btnH = 42, btnGap = 16;
    float btnTotalW = btnW * 2 + btnGap;
    float btnX  = dX + (dW - btnTotalW) * 0.5f;
    float btnY  = dY + dH - 60;

    if (GuiButton({btnX,              btnY, btnW, btnH}, "Yes, Exit")) return true;
    if (GuiButton({btnX + btnW + btnGap, btnY, btnW, btnH}, "No, Stay")) currentPage = 0;

    return false;
}

// =============================================================================
//  main()
// =============================================================================

int main()
{
    InitWindow(SW, SH, "Statistical NLP Text Classifier Engine");
    SetTargetFPS(60);

    // Load custom font (Consolas) from assets directory (handles both root and build dir runs)
    const char* fontPath = "assets/fonts/consola.ttf";
    if (!FileExists(fontPath))
    {
        fontPath = "../assets/fonts/consola.ttf";
    }
    g_font = LoadFontEx(fontPath, 64, nullptr, 0);
    GuiSetFont(g_font);

    // ── RayGUI style (applied before first frame) ─────────────────────────────
    GuiSetStyle(BUTTON,  BASE_COLOR_NORMAL,    ColorToInt(C_SURFACE_2));
    GuiSetStyle(BUTTON,  BORDER_COLOR_NORMAL,  ColorToInt(C_BORDER));
    GuiSetStyle(BUTTON,  TEXT_COLOR_NORMAL,    ColorToInt(C_TEXT_B));
    GuiSetStyle(BUTTON,  BASE_COLOR_FOCUSED,   ColorToInt(C_SURFACE_3));
    GuiSetStyle(BUTTON,  BORDER_COLOR_FOCUSED, ColorToInt(C_ACCENT));
    GuiSetStyle(BUTTON,  TEXT_COLOR_FOCUSED,   ColorToInt(C_TEXT_H));
    GuiSetStyle(BUTTON,  BASE_COLOR_PRESSED,   ColorToInt(C_ACCENT_DIM));
    GuiSetStyle(BUTTON,  BORDER_COLOR_PRESSED, ColorToInt(C_ACCENT));
    GuiSetStyle(BUTTON,  TEXT_COLOR_PRESSED,   ColorToInt(WHITE));

    GuiSetStyle(TEXTBOX, BASE_COLOR_NORMAL,    ColorToInt(C_SURFACE_2));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_NORMAL,  ColorToInt(C_BORDER));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_NORMAL,    ColorToInt(C_TEXT_B));
    GuiSetStyle(TEXTBOX, BASE_COLOR_FOCUSED,   ColorToInt(C_SURFACE_3));
    GuiSetStyle(TEXTBOX, BORDER_COLOR_FOCUSED, ColorToInt(C_ACCENT));
    GuiSetStyle(TEXTBOX, TEXT_COLOR_FOCUSED,   ColorToInt(C_TEXT_H));

    GuiSetStyle(DEFAULT, BACKGROUND_COLOR,     ColorToInt(C_BG));
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL,    ColorToInt(C_TEXT_B));
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);
    GuiSetStyle(DEFAULT, TEXT_SIZE, 18);

    // ── BUG-01 FIX: Load & train inside try/catch ─────────────────────────────
    // Previously an unhandled std::runtime_error crashed the process before any
    // Raylib window could draw.  Now the error is caught and displayed inside
    // the window so the user sees a clear, actionable message.
    std::string          g_loadError;
    bool                 g_ready = false;
    std::vector<Message> dataset;
    NaiveBayes           classifier;

    try
    {
        DataLoader loader;
        dataset   = loader.loadDataset("../data/SMSSpamCollection");
        classifier.train(dataset);
        g_ready   = true;
    }
    catch (const std::exception& ex)
    {
        g_loadError = ex.what();
    }

    // ── CPP-01 FIX: predict-page state lives in main, not as static globals ───
    int          currentPage = 0;
    PredictState predictState;

    // ── Main loop ─────────────────────────────────────────────────────────────
    while (!WindowShouldClose())
    {
        BeginDrawing();
        ClearBackground(C_BG);

        // Error state: show full-window error card, skip normal pages
        if (!g_ready)
        {
            if (DrawLoadError(g_loadError.c_str(), SW, SH))
            {
                // R-BUG-01 FIX: EndDrawing() must be called before break so
                // Raylib closes the current frame properly before the window
                // is destroyed by CloseWindow() after the loop.
                EndDrawing();
                break;
            }
            EndDrawing();
            continue;
        }

        DrawHeader(SW);

        switch (currentPage)
        {
            case 0:
                DrawMainMenu(currentPage, SW, SH);
                break;

            case 1:
                DrawPredictPage(currentPage, classifier, predictState, SW, SH);
                break;

            case 2:
                DrawStatisticsPage(currentPage, classifier, dataset, SW, SH);
                break;

            case 3:
                DrawAboutPage(currentPage, SW, SH);
                break;

            case 4:
                // R-LOGIC-01 FIX: GuiLock() disables all raygui control input
                // processing for the duration of the background DrawMainMenu call.
                // Without this, the four navigation buttons beneath the overlay
                // registered real clicks, causing currentPage to change to 1/2/3
                // in the same frame that the exit dialog was being drawn.
                GuiLock();
                DrawMainMenu(currentPage, SW, SH);   // visual background only
                GuiUnlock();
                if (DrawExitPage(currentPage, SW, SH))
                {
                    EndDrawing();
                    UnloadFont(g_font);
                    CloseWindow();
                    return 0;
                }
                break;

            default:
                currentPage = 0;
                break;
        }

        EndDrawing();
    }

    UnloadFont(g_font);
    CloseWindow();
    return 0;
}