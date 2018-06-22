#include <string.h>
#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "kolejka.h"
#include "plansza.h"
#include "fifo.h"

#define MAKS_DL_TEKSTU 100
#define MAX_POLE 361
#define MIN_POLE_NUM 20
#define MAX_POLE_NUM 340
#define GREEN_PIECE_BEGI_POS 188
#define RED_PIECE_BEGI_POS 172
#define SZEROKOSC 19
#define NUM_OF_WALLS 10

static PipesPtr potoki;
void zakoncz(GtkWidget *widget, gpointer data);
gboolean pobierz_tekst();
void przekaz_tekst(char *wykonany_ruch);

struct pole Board[MAX_POLE];
struct to_show_walls *TSWG, *TSWR;
struct to_show_move *TSM;
struct to_show_option *TSO;

bool ktos_wygral = 0;
int POS_RED = RED_PIECE_BEGI_POS, POS_GREEN = GREEN_PIECE_BEGI_POS;
enum COLOR moj, twoj;
GtkWidget *window;
Queue KOLEJ, QYELL;
Queue *Qwsk = &KOLEJ, *Qyell = &QYELL;
int SKOKI[4] = {-2, 2, -2 * SZEROKOSC, 2 * SZEROKOSC}, currBFS = 1;
int SWALL[4] = {-1, 1, -1 * SZEROKOSC, SZEROKOSC};
int BFSvst[MAX_POLE];

void pokazBlad(char *komunikat)
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(window), GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE, "%s", komunikat);
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
}

void czy_wygral()
{
    if (POS_GREEN % 19 == 1)
    {
        ktos_wygral = 1;
        pokazBlad("WYGRYWA ZIELONY!");
    }
    else if (POS_RED % 19 == 17)
    {
        ktos_wygral = 1;
        pokazBlad("WYGRYWA CZERWONY");
    }
}

void update_midd(int nr_pola, enum ORIEN orientacja)
{
    if (TSM->ruch == RED_PIECE)
    {
        TSWR->ile = TSWR->ile - 1;
        gtk_label_set_markup(GTK_LABEL(TSWR->num_label), liczby[TSWR->ile]);
    }
    else
    {
        TSWG->ile = TSWG->ile - 1;
        gtk_label_set_markup(GTK_LABEL(TSWG->num_label), liczby[TSWG->ile]);
    }
    if (orientacja == ORIEN_VERTICAL)
    {
        Board[nr_pola - 1].c_number = BR_VER;
        Board[nr_pola].c_number = BR_MID;
        Board[nr_pola + 1].c_number = BR_VER;
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola].color), "data/brown_middle.jpg");
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola - 1].color), "data/brown_vertical.jpg");
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola + 1].color), "data/brown_vertical.jpg");
    }
    else
    {
        Board[nr_pola - 19].c_number = BR_HOR;
        Board[nr_pola].c_number = BR_MID;
        Board[nr_pola + 19].c_number = BR_HOR;
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola].color), "data/brown_middle.jpg");
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola - 19].color), "data/brown_horizontal.jpg");
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola + 19].color), "data/brown_horizontal.jpg");
    }
}

void update_sqrt(int nr_pola, enum ORIEN orientacja)
{
    if (TSM->ruch == RED_PIECE)
    {
        Board[nr_pola].c_number = RED_PIECE;
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola].color), "data/red_piece.jpg");
        gtk_image_set_from_file(GTK_IMAGE(Board[POS_RED].color), "data/grey_sqrt.jpg");
        Board[POS_RED].c_number = GREY_SQR;
        POS_RED = nr_pola;
    }
    else
    {
        Board[nr_pola].c_number = GREEN_PIECE;
        gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola].color), "data/green_piece.jpg");
        gtk_image_set_from_file(GTK_IMAGE(Board[POS_GREEN].color), "data/grey_sqrt.jpg");
        Board[POS_GREEN].c_number = GREY_SQR;
        POS_GREEN = nr_pola;
    }
    czy_wygral();
}

void combo_selected(GtkWidget *wid, gpointer ptr)
{
    struct to_show_option *PTR = ptr;
    gchar *txt = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(PTR->combo_box));
    if (txt[1] == 'O')
        PTR->orientation = ORIEN_HORIZONTAL;
    else
        PTR->orientation = ORIEN_VERTICAL;
}

void ustaw_to_show_option(struct to_show_option *ptr, enum ORIEN h_v)
{
    ptr->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    ptr->label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(ptr->label), stale_napisy[4]);
    gtk_box_pack_start(GTK_BOX(ptr->box), ptr->label, FALSE, FALSE, 0);

    ptr->combo_box = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ptr->combo_box), stale_napisy[5]);
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(ptr->combo_box), stale_napisy[6]);
    g_signal_connect(G_OBJECT(ptr->combo_box), "changed", G_CALLBACK(combo_selected), (gpointer)ptr);

    gtk_box_pack_start(GTK_BOX(ptr->box), ptr->combo_box, FALSE, FALSE, 0);
    ptr->orientation = h_v;
}

void ustaw_to_show_move(struct to_show_move *ptr, enum COLOR piece)
{

    ptr->ruch = piece;
    ptr->label = gtk_label_new(NULL);
    if (piece == GREEN_PIECE)
        gtk_label_set_markup(GTK_LABEL(ptr->label), stale_napisy[3]);
    else
        gtk_label_set_markup(GTK_LABEL(ptr->label), stale_napisy[2]);
}
void update_to_show_move(struct to_show_move *ptr)
{
    if (ptr->ruch == GREEN_PIECE)
    {
        gtk_label_set_markup(GTK_LABEL(ptr->label), stale_napisy[2]);
        ptr->ruch = RED_PIECE;
    }
    else
    {
        ptr->ruch = GREEN_PIECE;
        gtk_label_set_markup(GTK_LABEL(ptr->label), stale_napisy[3]);
    }
}

void ustaw_to_show_walls(struct to_show_walls *ptr, enum COLOR piece)
{
    ptr->ile = NUM_OF_WALLS;
    ptr->const_label = gtk_label_new(NULL);
    ptr->num_label = gtk_label_new(NULL);
    ptr->box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 2);
    gtk_box_pack_start(GTK_BOX(ptr->box), ptr->const_label, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(ptr->box), ptr->num_label, TRUE, TRUE, 0);
    gtk_label_set_markup(GTK_LABEL(ptr->num_label), liczby[10]);
    if (piece == GREEN_PIECE)
        gtk_label_set_markup(GTK_LABEL(ptr->const_label), stale_napisy[1]);
    else
        gtk_label_set_markup(GTK_LABEL(ptr->const_label), stale_napisy[0]);
}

bool is_pole(int nr)
{
    if (nr >= MIN_POLE_NUM && nr <= MAX_POLE_NUM && (nr / 19) % 2 == 1 && (nr % 19) % 2 == 1)
        return true;
    return false;
}

bool IsReachable(int NR)
{
    int aim = 1;
    if (Board[NR].c_number == RED_PIECE)
        aim = 17;
    currBFS++;
    Clear(Qwsk);
    Add(NR, Qwsk);
    BFSvst[NR] = currBFS;
    while (Isempty(Qwsk) == 0)
    {
        int w;
        Get(&w, Qwsk);
        if (w % 19 == aim)
            return 1;
        for (int i = 0; i < 4; ++i)
        {
            int nowy = w + SKOKI[i];
            int poss_wall = w + SWALL[i];
            if (is_pole(nowy) && BFSvst[nowy] < currBFS && Board[nowy].c_number == GREY_SQR && (Board[poss_wall].c_number < BR_MID || Board[poss_wall].c_number > BR_HOR))
            {
                Add(nowy, Qwsk);
                BFSvst[nowy] = currBFS;
            }
        }
    }
    return 0;
}

void usun_yellow()
{

    while (Isempty(Qyell) == 0)
    {
        int w;
        Get(&w, Qyell);
        if (Board[w].c_number == YELLOW_SQR)
        {
            Board[w].c_number = GREY_SQR;
            gtk_image_set_from_file(GTK_IMAGE(Board[w].color), "data/grey_sqrt.jpg");
        }
    }
}

void middle_press(GtkWidget *ebox, GdkEvent *event, gpointer data)
{
    if (ktos_wygral)
        return;
    bool ustawil_sciane = 0;
    int nr_pola = (int)(gint *)data;
    if (moj != TSM->ruch)
    {
        pokazBlad("RUCH PRZECIWNIKA!");
        return;
    }
    if (Board[nr_pola].c_number == BR_MID)
    {
        pokazBlad("NIEDOZWOLONY RUCH!");
        return;
    }
    if (TSO->orientation == ORIEN_VERTICAL)
    {
        if (Board[nr_pola - 1].c_number == BR_VER || Board[nr_pola + 1].c_number == BR_VER)
        {
            pokazBlad("NIEDOZWOLONY RUCH!");
            return;
        }
        Board[nr_pola - 1].c_number = BR_VER;
        Board[nr_pola].c_number = BR_MID;
        Board[nr_pola + 1].c_number = BR_VER;
        if (IsReachable(POS_GREEN) && IsReachable(POS_RED))
        {
            gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola].color), "data/brown_middle.jpg");
            gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola - 1].color), "data/brown_vertical.jpg");
            gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola + 1].color), "data/brown_vertical.jpg");
            ustawil_sciane = 1;
        }
        else
        {
            pokazBlad("NIEDOZWOLONY RUCH!");
            Board[nr_pola - 1].c_number = BL_VER;
            Board[nr_pola].c_number = BL_MID;
            Board[nr_pola + 1].c_number = BL_VER;
        }
    }
    else
    {

        if (Board[nr_pola - 19].c_number == BR_HOR || Board[nr_pola + 19].c_number == BR_HOR)
        {
            pokazBlad("NIEDOZWOLONY RUCH!");
            return;
        }
        Board[nr_pola - 19].c_number = BR_HOR;
        Board[nr_pola].c_number = BR_MID;
        Board[nr_pola + 19].c_number = BR_HOR;
        if (IsReachable(POS_GREEN) && IsReachable(POS_RED))
        {
            gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola].color), "data/brown_middle.jpg");
            gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola - 19].color), "data/brown_horizontal.jpg");
            gtk_image_set_from_file(GTK_IMAGE(Board[nr_pola + 19].color), "data/brown_horizontal.jpg");
            ustawil_sciane = 1;
        }
        else
        {
            pokazBlad("NIEDOZWOLONY RUCH!");
            Board[nr_pola - 19].c_number = BL_VER;
            Board[nr_pola].c_number = BL_MID;
            Board[nr_pola + 19].c_number = BL_VER;
        }
    }
    if (ustawil_sciane)
    {
        char tmp[MAKS_DL_TEKSTU];
        char R[5] = "B";
        sprintf(tmp, "%s %d %d", R, nr_pola, TSO->orientation);
        przekaz_tekst(tmp);
        if (moj == RED_PIECE)
        {
            TSWR->ile = TSWR->ile - 1;
            gtk_label_set_markup(GTK_LABEL(TSWR->num_label), liczby[TSWR->ile]);
        }
        else
        {
            TSWG->ile = TSWG->ile - 1;
            gtk_label_set_markup(GTK_LABEL(TSWG->num_label), liczby[TSWG->ile]);
        }
        usun_yellow();
        update_to_show_move(TSM);
    }
}

void sqrt_press(GtkWidget *ebox, GdkEvent *event, gpointer data)
{
    int NR = (int)(gint *)data;
    if (ktos_wygral || (Board[NR].c_number != YELLOW_SQR && moj != Board[NR].c_number))
        return;
    if (Board[NR].c_number != YELLOW_SQR && Board[NR].c_number != RED_PIECE && Board[NR].c_number != GREEN_PIECE)
        return;
    if (moj != TSM->ruch)
    {
        pokazBlad("RUCH PRZECIWNIKA!");
        return;
    }
    if (Board[NR].c_number == YELLOW_SQR)
    {
        Board[NR].c_number = TSM->ruch;
        if (TSM->ruch == GREEN_PIECE)
        {
            gtk_image_set_from_file(GTK_IMAGE(Board[NR].color), "data/green_piece.jpg");
            gtk_image_set_from_file(GTK_IMAGE(Board[POS_GREEN].color), "data/grey_sqrt.jpg");
            Board[POS_GREEN].c_number = GREY_SQR;
            POS_GREEN = NR;
        }
        else
        {
            gtk_image_set_from_file(GTK_IMAGE(Board[NR].color), "data/red_piece.jpg");
            gtk_image_set_from_file(GTK_IMAGE(Board[POS_RED].color), "data/grey_sqrt.jpg");
            Board[POS_RED].c_number = GREY_SQR;
            POS_RED = NR;
        }
        char tmp[MAKS_DL_TEKSTU];
        char R[5] = "M";
        sprintf(tmp, "%s %d %d", R, NR, ORIEN_VERTICAL);
        przekaz_tekst(tmp);
        usun_yellow();
        update_to_show_move(TSM);
        czy_wygral();
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        int nowy = NR + SKOKI[i];
        int poss_wall = NR + SWALL[i];
        if (is_pole(nowy) && (Board[poss_wall].c_number < BR_MID || Board[poss_wall].c_number > BR_HOR) && Board[nowy].c_number == GREY_SQR)
        {
            Board[nowy].c_number = YELLOW_SQR;
            gtk_image_set_from_file(GTK_IMAGE(Board[nowy].color), "data/yellow_sqrt.jpg");
            Add(nowy, Qyell);
        }
    }
}

void PI()
{
    gtk_widget_show_all(window);
    gtk_main_quit();
}

int main(int argc, char *argv[])
{
    if ((potoki = initPipes(argc, argv)) == NULL)
        return 1;
    if (argc == 2 && strcmp(argv[1], "A") == 0)
    {
        moj = GREEN_PIECE;
        twoj = RED_PIECE;
    }
    else
    {
        moj = RED_PIECE;
        twoj = GREEN_PIECE;
    }

    gtk_init(&argc, &argv);
    Clear(Qyell);
    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (moj == GREEN_PIECE)
        gtk_window_set_title(GTK_WINDOW(window), "GTK - Quoridor GREEN");
    else
        gtk_window_set_title(GTK_WINDOW(window), "GTK - Quoridor RED");
    gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(window), 10);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(zakoncz), NULL);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_container_add(GTK_CONTAINER(window), box);

    GtkWidget *siata;
    siata = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(siata), 0);
    gtk_grid_set_column_spacing(GTK_GRID(siata), 0);

    gtk_box_pack_start(GTK_BOX(box), siata, TRUE, TRUE, 10);

    /// generowanie planszy ///////////////////////////////////////////////////////
    for (gint i = 0; i < MAX_POLE; ++i)
    {
        int x = i / SZEROKOSC, y = i % SZEROKOSC;
        Board[i].posX = x;
        Board[i].posY = y;
        Board[i].nr = i;
        Board[i].ebox = gtk_event_box_new();
        Board[i].color = gtk_image_new();
        gtk_container_add(GTK_CONTAINER(Board[i].ebox), Board[i].color);

        if (x % 2 == 1 && y % 2 == 1)
        {
            g_signal_connect(Board[i].ebox, "button_press_event", G_CALLBACK(sqrt_press), (gpointer)Board[i].nr);
        }

        if (x % 2 == 0 && y % 2 == 0)
        {
            Board[i].c_number = BL_MID;
            gtk_image_set_from_file(GTK_IMAGE(Board[i].color), "data/blue_middle.jpg");

            if (x == 0 || y == 0 || x == 18 || y == 18)
                ;
            else
                g_signal_connect(Board[i].ebox, "button_press_event", G_CALLBACK(middle_press), (gpointer)Board[i].nr);
        }
        else if (x % 2 == 0)
        {
            Board[i].c_number = BL_VER;
            gtk_image_set_from_file(GTK_IMAGE(Board[i].color), "data/blue_vertical.jpg");
        }
        else if (y % 2 == 0)
        {
            Board[i].c_number = BL_HOR;
            gtk_image_set_from_file(GTK_IMAGE(Board[i].color), "data/blue_horizontal.jpg");
        }
        else if (i == GREEN_PIECE_BEGI_POS)
        {
            Board[i].c_number = GREEN_PIECE;
            gtk_image_set_from_file(GTK_IMAGE(Board[i].color), "data/green_piece.jpg");
        }
        else if (i == RED_PIECE_BEGI_POS)
        {
            Board[i].c_number = RED_PIECE;
            gtk_image_set_from_file(GTK_IMAGE(Board[i].color), "data/red_piece.jpg");
        }
        else
        {
            Board[i].c_number = GREY_SQR;
            gtk_image_set_from_file(GTK_IMAGE(Board[i].color), "data/grey_sqrt.jpg");
        }
        gtk_grid_attach(GTK_GRID(siata), Board[i].ebox, Board[i].posX, Board[i].posY, 1, 1);
    }
    /// //////////////////////////////////////////////////////////////////////////

    ///generowanie subboxa z opcjami i info
    GtkWidget *subbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_pack_start(GTK_BOX(box), subbox, TRUE, TRUE, 0);

    TSWR = (struct to_show_walls *)malloc(sizeof(struct to_show_walls));
    ustaw_to_show_walls(TSWR, RED_PIECE);
    gtk_box_pack_start(GTK_BOX(subbox), TSWR->box, TRUE, FALSE, 0);

    /// FALSE, FALSE,
    TSM = (struct to_show_move *)malloc(sizeof(struct to_show_move));
    ustaw_to_show_move(TSM, GREEN_PIECE);
    gtk_box_pack_start(GTK_BOX(subbox), TSM->label, TRUE, FALSE, 0);

    TSO = (struct to_show_option *)malloc(sizeof(struct to_show_option));
    ustaw_to_show_option(TSO, ORIEN_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(subbox), TSO->box, TRUE, FALSE, 0);

    TSWG = (struct to_show_walls *)malloc(sizeof(struct to_show_walls));
    ustaw_to_show_walls(TSWG, GREEN_PIECE);
    gtk_box_pack_start(GTK_BOX(subbox), TSWG->box, TRUE, FALSE, 0);
    /// ////////////////////////////////////////////////////////////

    g_timeout_add(100, pobierz_tekst, NULL);

    GtkWidget *WD;

    WD = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    if (moj == GREEN_PIECE)
        gtk_window_set_title(GTK_WINDOW(WD), "GTK - Quoridor GREEN");
    else
        gtk_window_set_title(GTK_WINDOW(WD), "GTK - Quoridor RED");
    gtk_window_set_position(GTK_WINDOW(WD), GTK_WIN_POS_CENTER);
    gtk_container_set_border_width(GTK_CONTAINER(WD), 10);

    g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(PI), NULL);

    gtk_widget_show_all(WD);
    gtk_main();

    return 0;
}

void zakoncz(GtkWidget *widget, gpointer data)
{
    if (ktos_wygral == 0)
        przekaz_tekst("ZAKONCZ");
    closePipes(potoki);
    gtk_main_quit();
}

void zakonczyl()
{
    pokazBlad("Przeciwnik zrezygnowal");
    ktos_wygral = 1;
}
gboolean pobierz_tekst()
{

    gchar wejscie[MAKS_DL_TEKSTU + 5];

    if (getStringFromPipe(potoki, wejscie, MAKS_DL_TEKSTU))
    {
        int nr_pola;
        enum ORIEN orientacja;
        char tmp[MAKS_DL_TEKSTU];
        sscanf(wejscie, "%s %d %d", tmp, &nr_pola, &orientacja);
        if (tmp[0] == 'B')
            update_midd(nr_pola, orientacja); /// wstawiono sciane
        if (tmp[0] == 'M')
            update_sqrt(nr_pola, orientacja); /// ruszono pionkiem
        if (tmp[0] == 'Z')
            zakonczyl(); /// gracz zrezygnowal z dalszej gry

        update_to_show_move(TSM); /// to musi! byc na koncu!
    }
    return TRUE;
}
void przekaz_tekst(char *wykonany_ruch)
{
    sendStringToPipe(potoki, wykonany_ruch);
}
