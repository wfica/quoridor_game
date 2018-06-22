#include <gtk/gtk.h>

enum COLOR{
    BL_MID=0,
    BL_VER=1,
    BL_HOR=2,
    BR_MID=3,
    BR_VER=4,
    BR_HOR=5,
    GREY_SQR=6,
    YELLOW_SQR=7,
    GREEN_PIECE=8,
    RED_PIECE=9
} ;

enum ORIEN{
    ORIEN_VERTICAL=0,
    ORIEN_HORIZONTAL=1
};

char *liczby[11] = {
    "<span font='15'>0</span>",
    "<span font='15'>1</span>",
    "<span font='15'>2</span>",
    "<span font='15'>3</span>",
    "<span font='15'>4</span>",
    "<span font='15'>5</span>",
    "<span font='15'>6</span>",
    "<span font='15'>7</span>",
    "<span font='15'>8</span>",
    "<span font='15'>9</span>",
    "<span font='15'>10</span>"
};

const char *stale_napisy[]={
    "<span bgcolor='#f00' font='15'>Sciany czerwonego: </span>",
    "<span bgcolor='#0f0' font='15'>Sciany zielonego: </span>",
    "<span bgcolor='#f00' font='15'>Ruch czerwonego...</span>",
    "<span bgcolor='#0f0' font='15'>Ruch zielonego...</span>",
    "<span font='15'>Ustawienie sciany</span>",
    "POZIOMA",
    "PIONOWA"
};

struct pole{
    int posX, posY, nr;
    enum COLOR c_number;
    GtkWidget *color;
    GtkWidget *ebox;
};

struct to_show_walls{
    int ile;
    GtkWidget *const_label;
    GtkWidget *num_label;
    GtkWidget *box;
};

struct to_show_move{
    enum COLOR ruch;
    GtkWidget *label;
};

struct to_show_option{
    GtkWidget *box;
    GtkWidget *label;
    GtkWidget *combo_box;
    enum ORIEN orientation;
};
