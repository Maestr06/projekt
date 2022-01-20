#include <gtk/gtk.h>
#include <stdbool.h>
#include <string.h>
#define SIZE 255

const char filename[] = "warehouse111.txt";

int position = 0, count = 0, iter = 0;

struct car
{
    char brand[SIZE];
    char model[SIZE];
    char color[SIZE];

    int price;
    int hp;
    int amount;
};

typedef struct entries
{
    GtkWidget *brand, *model, *color, *price, *hp, *amount;
} Entries;

GtkWidget *brand, *model, *color, *price, *hp, *amount;

struct car warehouse[SIZE];

struct car list[SIZE];

static bool load_cars_from_file(const char *filename, struct car *warehouse);

static bool write_cars_to_file(const char *filename, struct car *warehouse);

static void my_split(const char *source, char *brand, char *model, char *color, char *price, char *hp, char *amount);

static void make_pages(GtkWidget *notebook, GtkWidget *grid, GtkWidget *name, GtkWidget **brand, GtkWidget **model, GtkWidget **color, GtkWidget **price, GtkWidget **hp, GtkWidget **amount);

static void PageSwitch(GtkWidget *widget, GtkNotebookTab *page, gint page_num)
{
    printf("%d\n", page_num);
}

static void add_car(GtkWidget *widget, gpointer data);

static bool car_exists(const char *brand, const char *model,
                       const char *color, const char *price, const char *hp);

static void find_car(GtkWidget *widget, gpointer data);

static void delete_car(GtkWidget *widget, gpointer data);

static void next_page(GtkWidget *widget, gpointer data);

static void prev_page(GtkWidget *widget, gpointer data);

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window, *button, *grid, *notebook, *label;

    Entries *data = (Entries *)malloc(sizeof(Entries));

    GtkAdjustment *adjustment;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "Car warehouse");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);

    notebook = gtk_notebook_new();
    gtk_container_add(GTK_CONTAINER(window), notebook);

    // adding a page to notebook for adding items

    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    label = gtk_label_new("Add");
    make_pages(notebook, grid, label, &brand, &model, &color, &price, &hp, &amount);

    data->brand = brand;
    data->model = model;
    data->color = color;
    data->amount = amount;
    data->hp = hp;
    data->price = price;

    button = gtk_button_new_with_label("Add this position");
    g_signal_connect(button, "clicked", G_CALLBACK(add_car), data);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 7, 2, 1);

    // adding a page to notebook for finding items

    Entries *find_data = (Entries *)malloc(sizeof(Entries));

    grid = gtk_grid_new();
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);
    gtk_grid_set_row_homogeneous(GTK_GRID(grid), TRUE);
    label = gtk_label_new("Find");
    make_pages(notebook, grid, label, &brand, &model, &color, &price, &hp, &amount);

    find_data->brand = brand;
    find_data->model = model;
    find_data->color = color;
    find_data->amount = amount;
    find_data->hp = hp;
    find_data->price = price;

    button = gtk_button_new_with_label("Prev position");
    g_signal_connect(button, "clicked", G_CALLBACK(prev_page), find_data);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 7, 1, 1);

    button = gtk_button_new_with_label("Next position");
    g_signal_connect(button, "clicked", G_CALLBACK(next_page), find_data);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 7, 1, 1);

    button = gtk_button_new_with_label("Search (Brand/Model/Color)");
    g_signal_connect(button, "clicked", G_CALLBACK(find_car), find_data);
    gtk_grid_attach(GTK_GRID(grid), button, 0, 8, 1, 1);

    button = gtk_button_new_with_label("Delete current");
    g_signal_connect(button, "clicked", G_CALLBACK(delete_car), find_data);
    gtk_grid_attach(GTK_GRID(grid), button, 1, 8, 1, 1);

    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), 1);
    g_signal_connect_swapped(notebook, "switch_page", G_CALLBACK(PageSwitch), notebook);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv)
{

    FILE *fp;
    fp = fopen(filename, "a");
    fclose(fp);
    GtkApplication *app;
    int status;

    load_cars_from_file(filename, warehouse);

    app = gtk_application_new("com.github.Maestr06.projekt", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}

static void next_page(GtkWidget *widget, gpointer data)
{
    if (count > 1)
    {
        if (iter == count - 1)
        {
            iter = 0;
        }
        else
        {
            iter++;
        }
        Entries *d = data;
        GtkWidget *b = d->brand;
        GtkWidget *m = d->model;
        GtkWidget *c = d->color;
        GtkWidget *p = d->price;
        GtkWidget *h = d->hp;
        GtkWidget *a = d->amount;

        const gchar *brand, *model, *color, *price, *hp, *amount;

        char price1[24];
        char hp1[24];
        char amount1[24];
        sprintf(price1, "%d", list[iter].price);
        sprintf(hp1, "%d", list[iter].hp);
        sprintf(amount1, "%d", list[iter].amount);

        gtk_entry_set_text(GTK_ENTRY(b), list[iter].brand);
        gtk_entry_set_text(GTK_ENTRY(m), list[iter].model);
        gtk_entry_set_text(GTK_ENTRY(c), list[iter].color);
        gtk_entry_set_text(GTK_ENTRY(p), price1);
        gtk_entry_set_text(GTK_ENTRY(h), hp1);
        gtk_entry_set_text(GTK_ENTRY(a), amount1);
    }
}

static void prev_page(GtkWidget *widget, gpointer data)
{
    if (count > 1)
    {
        if (iter == 0)
        {
            iter = count - 1;
        }
        else
        {
            iter--;
        }
        Entries *d = data;
        GtkWidget *b = d->brand;
        GtkWidget *m = d->model;
        GtkWidget *c = d->color;
        GtkWidget *p = d->price;
        GtkWidget *h = d->hp;
        GtkWidget *a = d->amount;

        const gchar *brand, *model, *color, *price, *hp, *amount;

        char price1[24];
        char hp1[24];
        char amount1[24];
        sprintf(price1, "%d", list[iter].price);
        sprintf(hp1, "%d", list[iter].hp);
        sprintf(amount1, "%d", list[iter].amount);

        gtk_entry_set_text(GTK_ENTRY(b), list[iter].brand);
        gtk_entry_set_text(GTK_ENTRY(m), list[iter].model);
        gtk_entry_set_text(GTK_ENTRY(c), list[iter].color);
        gtk_entry_set_text(GTK_ENTRY(p), price1);
        gtk_entry_set_text(GTK_ENTRY(h), hp1);
        gtk_entry_set_text(GTK_ENTRY(a), amount1);
    }
}

static void delete_car(GtkWidget *widget, gpointer data)
{
    Entries *d = data;
    GtkWidget *b = d->brand;
    GtkWidget *m = d->model;
    GtkWidget *c = d->color;
    GtkWidget *p = d->price;
    GtkWidget *h = d->hp;
    GtkWidget *a = d->amount;

    const gchar *brand, *model, *color, *price, *hp, *amount;
    brand = gtk_entry_get_text(GTK_ENTRY(b));
    model = gtk_entry_get_text(GTK_ENTRY(m));
    color = gtk_entry_get_text(GTK_ENTRY(c));
    price = gtk_entry_get_text(GTK_ENTRY(p));
    hp = gtk_entry_get_text(GTK_ENTRY(h));
    amount = gtk_entry_get_text(GTK_ENTRY(a));

    struct car temp[SIZE];
    struct car *t = warehouse;
    int j = 0;

    for (int i = 0; i < SIZE; i++)
    {
        char price1[24];
        char hp1[24];
        sprintf(price1, "%d", warehouse[i].price);
        sprintf(hp1, "%d", warehouse[i].hp);

        if (strcmp(warehouse[i].brand, brand) == 0 &&
            strcmp(warehouse[i].model, model) == 0 &&
            strcmp(warehouse[i].color, color) == 0 &&
            strcmp(price1, price) == 0 &&
            strcmp(hp1, hp) == 0)
        {
            warehouse[i].amount = 0;
        }
        else
        {
            temp[j] = warehouse[i];
            j++;
        }
    }
    position = 0;
    for (int k = 0; k < SIZE; k++)
    {
        warehouse[k] = temp[k];
        position++;
    }
    write_cars_to_file(filename, warehouse);
    // load_cars_from_file(filename, warehouse);

    char price1[24];
    char hp1[24];
    char amount1[24];
    sprintf(price1, "%d", warehouse[0].price);
    sprintf(hp1, "%d", warehouse[0].hp);
    sprintf(amount1, "%d", warehouse[0].amount);

    gtk_entry_set_text(GTK_ENTRY(b), "");
    gtk_entry_set_text(GTK_ENTRY(m), "");
    gtk_entry_set_text(GTK_ENTRY(c), "");
    gtk_entry_set_text(GTK_ENTRY(p), "1000");
    gtk_entry_set_text(GTK_ENTRY(h), "10");
    gtk_entry_set_text(GTK_ENTRY(a), "1");
    iter = 0;
    count = 0;
}

static void find_car(GtkWidget *widget, gpointer data)
{
    memset(list, 0, sizeof(list));
    iter = 0;
    count = 0;
    Entries *d = data;
    GtkWidget *b = d->brand;
    GtkWidget *m = d->model;
    GtkWidget *c = d->color;
    GtkWidget *p = d->price;
    GtkWidget *h = d->hp;
    GtkWidget *a = d->amount;

    const gchar *brand, *model, *color, *price, *hp, *amount;
    brand = gtk_entry_get_text(GTK_ENTRY(b));
    model = gtk_entry_get_text(GTK_ENTRY(m));
    color = gtk_entry_get_text(GTK_ENTRY(c));
    price = gtk_entry_get_text(GTK_ENTRY(p));
    hp = gtk_entry_get_text(GTK_ENTRY(h));
    amount = gtk_entry_get_text(GTK_ENTRY(a));

    if (strcmp(brand, "") != 0 ||
        strcmp(model, "") != 0 ||
        strcmp(color, "") != 0)
    {
        for (int i = 0; i < SIZE; i++)
        {
            int same_brand = strcmp(warehouse[i].brand, brand);
            int same_model = strcmp(warehouse[i].model, model);
            int same_color = strcmp(warehouse[i].color, color);

            if (same_brand == 0 && same_model == 0 && same_color == 0)
            {
                list[count] = warehouse[i];
                count++;
            }
            else if ((same_brand == 0 && same_model == 0 && strcmp(color, "") == 0) ||
                     (same_model == 0 && same_color == 0 && strcmp(brand, "") == 0) ||
                     (same_brand == 0 && same_color == 0 && strcmp(model, "") == 0))
            {
                list[count] = warehouse[i];
                count++;
            }
            else if (same_brand == 0 && strcmp(model, "") == 0 && strcmp(color, "") == 0)
            {
                list[count] = warehouse[i];
                count++;
            }
            else if (strcmp(brand, "") == 0 && same_model == 0 && strcmp(color, "") == 0)
            {
                list[count] = warehouse[i];
                count++;
            }
            else if (strcmp(brand, "") == 0 && strcmp(model, "") == 0 && same_color == 0)
            {
                list[count] = warehouse[i];
                count++;
            }
        }
        if (count > 0)
        {
            char price1[24];
            char hp1[24];
            char amount1[24];
            sprintf(price1, "%d", list[0].price);
            sprintf(hp1, "%d", list[0].hp);
            sprintf(amount1, "%d", list[0].amount);

            g_print("%s;%s;%s;%s;%s;%s;\n", list[0].brand, list[0].model, list[0].color, price1, hp1, amount1);

            gtk_entry_set_text(GTK_ENTRY(b), list[0].brand);
            gtk_entry_set_text(GTK_ENTRY(m), list[0].model);
            gtk_entry_set_text(GTK_ENTRY(c), list[0].color);
            gtk_entry_set_text(GTK_ENTRY(p), price1);
            gtk_entry_set_text(GTK_ENTRY(h), hp1);
            gtk_entry_set_text(GTK_ENTRY(a), amount1);
        }
        else
        {
            gtk_entry_set_text(GTK_ENTRY(b), "");
            gtk_entry_set_text(GTK_ENTRY(m), "");
            gtk_entry_set_text(GTK_ENTRY(c), "");
        }
    }
}

void add_car(GtkWidget *widget, gpointer data)
{
    FILE *fp;
    fp = fopen(filename, "a");
    Entries *d = data;
    GtkWidget *b = d->brand;
    GtkWidget *m = d->model;
    GtkWidget *c = d->color;
    GtkWidget *a = d->amount;
    GtkWidget *h = d->hp;
    GtkWidget *p = d->price;
    const gchar *brand, *model, *color, *price, *hp, *amount;
    brand = gtk_entry_get_text(GTK_ENTRY(b));
    model = gtk_entry_get_text(GTK_ENTRY(m));
    color = gtk_entry_get_text(GTK_ENTRY(c));
    price = gtk_entry_get_text(GTK_ENTRY(p));
    hp = gtk_entry_get_text(GTK_ENTRY(h));
    amount = gtk_entry_get_text(GTK_ENTRY(a));
    char text[1024] = "";
    strcat(text, brand);
    strcat(text, ";");
    strcat(text, model);
    strcat(text, ";");
    strcat(text, color);
    strcat(text, ";");
    strcat(text, price);
    strcat(text, ";");
    strcat(text, hp);
    strcat(text, ";");
    strcat(text, amount);
    strcat(text, ";");
    strcat(text, "\n");
    if (strcmp(brand, "") == 0 ||
        strcmp(model, "") == 0 ||
        strcmp(color, "") == 0)
    {
        g_print("Error, cant input empty brand/model/color\n");
    }
    else if (car_exists(brand, model, color, price, hp))
    {
        int id = 0;
        for (int i = 0; i < SIZE; i++)
        {
            char price1[24];
            char hp1[24];
            sprintf(price1, "%d", warehouse[i].price);
            sprintf(hp1, "%d", warehouse[i].hp);

            if (strcmp(warehouse[i].brand, brand) == 0 &&
                strcmp(warehouse[i].model, model) == 0 &&
                strcmp(warehouse[i].color, color) == 0 &&
                strcmp(price1, price) == 0 &&
                strcmp(hp1, hp) == 0)
            {
                warehouse[i].amount += atoi(amount);
            }
        }
        write_cars_to_file(filename, warehouse);
    }
    else
    {
        fputs(text, fp);

        // g_print("%d\n", position);
        struct car car1;
        strcpy(car1.brand, brand);
        strcpy(car1.model, model);
        strcpy(car1.color, color);
        car1.price = atoi(price);
        car1.hp = atoi(hp);
        car1.amount = atoi(amount);
        warehouse[position] = car1;
        position++;
    }
    fclose(fp);
}

static bool car_exists(const char *brand, const char *model,
                       const char *color, const char *price, const char *hp)
{
    for (int i = 0; i < SIZE; i++)
    {
        char price1[24];
        char hp1[24];
        sprintf(price1, "%d", warehouse[i].price);
        sprintf(hp1, "%d", warehouse[i].hp);

        if (strcmp(warehouse[i].brand, brand) == 0 &&
            strcmp(warehouse[i].model, model) == 0 &&
            strcmp(warehouse[i].color, color) == 0 &&
            strcmp(price1, price) == 0 &&
            strcmp(hp1, hp) == 0)
        {
            return 1;
        }
    }
    return 0;
}

static void my_split(const char *source, char *brand, char *model, char *color, char *price, char *hp, char *amount)
{
    while (*source != ';')
    {
        *brand++ = *source++;
    }
    *brand = '\0';
    *source++;
    while (*source != ';')
    {
        *model++ = *source++;
    }
    *source++;
    *model = '\0';
    while (*source != ';')
    {
        *color++ = *source++;
    }
    *color = '\0';
    *source++;
    while (*source != ';')
    {
        *price++ = *source++;
    }
    *price = '\0';
    *source++;
    while (*source != ';')
    {
        *hp++ = *source++;
    }
    *hp = '\0';
    *source++;
    while (*source != ';')
    {
        *amount++ = *source++;
    }
    *amount = '\0';
}

static bool write_cars_to_file(const char *filename, struct car *warehouse)
{
    FILE *fp;
    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        printf("No such file to read from\n");
        return 0;
    }
    for (int i = 0; i < SIZE; i++)
    {
        char price1[24];
        char hp1[24];
        char amount1[24];
        sprintf(price1, "%d", warehouse[i].price);
        sprintf(hp1, "%d", warehouse[i].hp);
        sprintf(amount1, "%d", warehouse[i].amount);

        if (warehouse[i].amount > 0)
        {
            char text[1024] = "";
            strcat(text, warehouse[i].brand);
            strcat(text, ";");
            strcat(text, warehouse[i].model);
            strcat(text, ";");
            strcat(text, warehouse[i].color);
            strcat(text, ";");
            strcat(text, price1);
            strcat(text, ";");
            strcat(text, hp1);
            strcat(text, ";");
            strcat(text, amount1);
            strcat(text, ";");
            strcat(text, "\n");
            fputs(text, fp);
        }
    }
    fclose(fp);
    return 1;
}

static bool load_cars_from_file(const char *filename, struct car *warehouse)
{
    FILE *fp;
    fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("No such file to read from\n");
        return 0;
    }

    char napis[SIZE];

    for (int i = 0; i < SIZE; i++)
    {
        //result = fgets (napis, SIZE, stdin);// czytamy ze standardowego wejścia
        char *result;
        result = fgets(napis, SIZE, fp);
        if (result != NULL)
        {
            //printf ("%d %s", i, napis);
            char brand[SIZE];
            char model[SIZE];
            char color[SIZE];
            char amount[SIZE];
            char hp[SIZE];
            char price[SIZE];

            my_split(napis, brand, model, color, price, hp, amount);

            strcpy(warehouse[i].brand, brand);
            strcpy(warehouse[i].model, model);
            strcpy(warehouse[i].color, color);
            int val = atoi(amount);
            warehouse[i].amount = val;
            int val1 = atoi(hp);
            warehouse[i].hp = val1;
            int val2 = atoi(price);
            warehouse[i].price = val2;

            position++;
        }
    }
    fclose(fp);
    return 1;
}

static void make_pages(GtkWidget *notebook, GtkWidget *grid, GtkWidget *name, GtkWidget **brand, GtkWidget **model, GtkWidget **color, GtkWidget **price, GtkWidget **hp, GtkWidget **amount)
{
    GtkWidget *label;
    GtkAdjustment *adjustment;

    gtk_notebook_append_page(GTK_NOTEBOOK(notebook), grid, name);

    *brand = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(*brand), 255);
    *model = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(*model), 255);
    *color = gtk_entry_new();
    gtk_entry_set_max_length(GTK_ENTRY(*color), 255);

    adjustment = gtk_adjustment_new(1000.0, 1000.0, 100000.0, 1.0, 5.0, 0.0);
    *price = gtk_spin_button_new(adjustment, 1.0, 0);
    adjustment = gtk_adjustment_new(10.0, 10.0, 2000.0, 1.0, 5.0, 0.0);
    *hp = gtk_spin_button_new(adjustment, 1.0, 0);
    adjustment = gtk_adjustment_new(1.0, 1.0, 20.0, 1.0, 5.0, 0.0);
    *amount = gtk_spin_button_new(adjustment, 1.0, 0);

    label = gtk_label_new("Brand:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 1, 1, 1);
    label = gtk_label_new("Model:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 2, 1, 1);
    label = gtk_label_new("Color:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 3, 1, 1);
    label = gtk_label_new("Price:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 4, 1, 1);
    label = gtk_label_new("Horsepower:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 5, 1, 1);
    label = gtk_label_new("Amount:");
    gtk_grid_attach(GTK_GRID(grid), label, 0, 6, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), *brand, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), *model, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), *color, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), *price, 1, 4, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), *hp, 1, 5, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), *amount, 1, 6, 1, 1);
}
