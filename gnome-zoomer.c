#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dconf/client/dconf-client.h>
#include <glib/gvariant.h>

#define COLOUR_RED "\033[1;31m"
#define COLOUR_GREEN "\033[1;32m"
#define COLOUR_YELLOW "\033[1;33m"
#define COLOUR_BLUE "\033[1;34m"
#define COLOUR_PURPLE "\033[1;35m"
#define COLOUR_CYAN "\033[1;36m"
#define COLOUR_RESET "\033[0m"

void print_usage() {
    printf(
        "GNOME Zoomer by Fotis Gimian\n"
        "\n"
        COLOUR_GREEN
        "The GNOME zoomer utility updates the relevant dconf setting to zoom the entire\n"
        "desktop by a chosen increment.  This may be easily assigned to a keyboard\n"
        "shortcut in GNOME so that the user may zoom in with shortcuts using their\n"
        "chosen increment.\n"
        COLOUR_RESET
        "\n"
        COLOUR_BLUE "Usage: gnome-zoomer <increment>\n" COLOUR_RESET
        "\n"
        COLOUR_YELLOW
        "The <increment> value must be a decimal number between the range -31 and 31 where a 10%% increment\n"
        "would be represented by 0.1.\n"
        COLOUR_RESET
    );
}

void print_error(char *error) {
    printf(COLOUR_RED "Error: %s\n" COLOUR_RESET "\n", error);
}

void print_error_and_usage(char *error) {
    print_error(error);
    print_usage();
}

int main(int argc, char *argv[]) {
    // Validate command line arguments and the <increment> value.
    if (argc != 2) {
        print_usage();
        return 1;
    }

    char *endptr;
    double mag_increment = strtod(argv[1], &endptr);

    if (mag_increment == 0 && errno == ERANGE) {
        print_error_and_usage("The increment entered was out of range.");
        return 1;
    }

    if (strcmp(endptr, "") != 0) {
        print_error_and_usage("An invalid increment was entered.");
        return 1;
    }

    if (mag_increment < -31 || mag_increment > 31) {
        print_error_and_usage("An increment was entered outside the range -31 to 31.");
        return 1;
    }

    // Obtain the current magnification factor.
    DConfClient *client = dconf_client_new();
    GVariant *mag_factor = dconf_client_read(
        client,
        "/org/gnome/desktop/a11y/magnifier/mag-factor"
    );
    gdouble current_mag_factor = mag_factor == NULL ? 1.0 : g_variant_get_double(mag_factor);

    // Determine the new magnification factor based on the increment requested.
    gdouble new_mag_factor = current_mag_factor + mag_increment;
    if (new_mag_factor < 1.0) {
        new_mag_factor = 1.0;
    }

    // Update the magnification factor and enable the magnifier.
    GError *mag_factor_error = NULL;
    dconf_client_write_fast(
        client,
        "/org/gnome/desktop/a11y/magnifier/mag-factor",
        g_variant_new_double(new_mag_factor),
        &mag_factor_error
    );

    GError *mag_enabled_error = NULL;
    bool mag_enabled = new_mag_factor != 1.0;
    dconf_client_write_fast(
        client,
        "/org/gnome/desktop/a11y/applications/screen-magnifier-enabled",
        g_variant_new_boolean(mag_enabled),
        &mag_enabled_error
    );

    dconf_client_sync(client);

    // Handle any errors that may have occurred while updating dconf settings.
    bool dconf_error_occurred = false;
    if (mag_factor_error != NULL) {
        char *error = NULL;
        sprintf(error, "Unable to set magnification factor: %s", mag_factor_error->message);
        print_error(error);
        g_error_free(mag_factor_error);
        dconf_error_occurred = true;
    }

    if (mag_enabled_error != NULL) {
        char *error = NULL;
        sprintf(error, "Unable to enable magnifier: %s", mag_enabled_error->message);
        print_error(error);
        g_error_free(mag_enabled_error);
        dconf_error_occurred = true;
    }

    if (dconf_error_occurred) {
        return 2;
    }

    return 0;
}
