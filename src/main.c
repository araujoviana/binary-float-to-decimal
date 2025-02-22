/**
 * @mainpage BinaryFloatToDecimal Documentation
 *
 * @section intro_sec Introduction
 * This document describes a simple C program that converts a binary
 * representation of a single-precision floating-point number (IEEE 754
 * standard) into its decimal equivalent.
 *
 * The program takes a 32-bit binary string as input from the user, parses it
 * into its sign, exponent, and fraction components, and then calculates and
 * prints the corresponding decimal floating-point value.
 *
 *
 * @section usage_sec Usage
 * To generate and use the documentation for this program within a CMake
 * project:
 *
 * 1. **Navigate to the `build` directory:**
 *    ```bash
 *    cd build
 *    ```
 *    *(Assuming you are in the project's root directory where `CMakeLists.txt`
 * is located)*
 *
 * 2. **Configure the project using CMake:**
 *    ```bash
 *    cmake ..
 *    ```
 *    *This command configures the build based on the `CMakeLists.txt` in the
 * parent directory. CMake will use `Doxyfile.in` to generate a `Doxyfile` in
 * the `build` directory.*
 *
 * 3. **Build the documentation target (doc):**
 *    ```bash
 *    make doc
 *    ```
 *    *This command specifically builds the 'doc' target defined in your CMake
 * configuration, which will run Doxygen to generate the documentation.*
 *
 * 4. **Open the generated documentation:**
 *    The HTML documentation will be located in the `html` subdirectory within
 * the `doc/doxygen` directory inside your `build` directory:
 *    ```bash
 *    firefox doc/doxygen/html/index.html
 *    ```
 *    *Replace `firefox` with your preferred web browser.*
 *
 * 5. **Run the compiled executable (optional):**
 *    To compile and run the program itself, you can use:
 *    ```bash
 *    make
 *    ./BinaryFloatToDecimal
 *    ```
 *    *The executable `BinaryFloatToDecimal` will be created in the `build`
 * directory.*
 *
 * @note The `Doxyfile` used for generating documentation is configured through
 * `Doxyfile.in` and CMake. Ensure that `Doxyfile.in` is correctly set up to
 * include your source files and desired Doxygen settings. *
 *
 * @author Matheus Araujo
 * @date 22/02/2025
 */

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @brief Splits a binary float string into sign, exponent, and fraction parts.
 *
 * Extracts the sign bit, exponent bits (8), and fraction bits (23) from a
 * binary string representing a single-precision float (IEEE 754).
 *
 * @param binary_float String of '0's and '1's (32 bits) for a binary float.
 * @return char** Array of 3 strings: [sign, exponent, fraction] if successful.
 *         Returns NULL on memory allocation error. The caller is responsible
 * for freeing the allocated memory for each string and the array itself.
 * @note Input string is expected to be 32 bits long. Memory allocation failure
 *       results in a NULL return and an error message printed to stderr.
 */
char **split_binary_float(char *binary_float);

/**
 * @brief Parses a binary string to a float value.
 *
 * Converts a string of '0's and '1's into its corresponding float value.
 * Handles both integer and fractional binary representations based on the
 * `is_fractional` flag.
 *
 * @param binary_string String of '0's and '1's representing a binary number.
 * @param is_fractional Integer flag to indicate fractional conversion.
 *                      If non-zero, the binary string is treated as a
 * fractional part (bits after binary point). If 0, the binary string is treated
 * as an integer.
 * @return float The float value represented by the binary string.
 * @note For fractional conversion, each bit is multiplied by decreasing powers
 * of 0.5 (1/2, 1/4, 1/8, ...).
 */
float parse_bits(char *binary_string, int is_fractional);

/**
 * @brief Converts IEEE 754 single-precision float parts to a decimal double.
 *
 * Takes the sign, exponent, and fraction parts of a binary IEEE 754 float
 * (as strings) and converts them into a decimal `double` value.
 *
 * @param full_float Array of 3 strings representing: [sign, exponent, fraction]
 *                   from a split binary IEEE 754 float, typically the output of
 * `split_binary_float`.
 * @return double The decimal `double` representation of the IEEE float.
 *         Returns 0.0 and prints an error message to stderr if the exponent is
 * 255 (which in IEEE 754 standard represents NaN or Infinity).
 * @note Expects `full_float` to be the output of `split_binary_float`.
 *       Handles subnormal numbers (exponent is 0) according to IEEE 754
 * standard.
 */
double convert_ieee_float(char **full_float);

/**
 * @brief Main function of the binary float to decimal converter program.
 *
 * Prompts the user to enter a 32-bit binary floating-point number,
 * converts it to its decimal representation, and prints the result.
 *
 * @param argc Integer argument count.
 * @param argv Character array of argument strings.
 * @return int Returns 0 if the program executes successfully.
 */
int main(int argc, char *argv[]) {
  printf("Insert the binary float: ");

  char user_binary_float[33];
  scanf("%s", user_binary_float);

  char **parsed_float = split_binary_float(user_binary_float);

  double decimal_float = convert_ieee_float(parsed_float);

  printf("Result: %f\n", decimal_float);

  free(parsed_float[0]);
  free(parsed_float[1]);
  free(parsed_float[2]);
  free(parsed_float);

  return 0;
}

/**
 * @brief Splits a binary float string into sign, exponent, and fraction parts.
 *
 * Extracts the sign bit, exponent bits (8), and fraction bits (23) from a
 * binary string representing a single-precision float (IEEE 754).
 *
 * @param binary_float String of '0's and '1's (32 bits) for a binary float.
 * @return char** Array of 3 strings: [sign, exponent, fraction] if successful.
 *         Returns NULL on memory allocation error. The caller is responsible
 * for freeing the allocated memory for each string and the array itself.
 * @note Input string is expected to be 32 bits long. Memory allocation failure
 *       results in a NULL return and an error message printed to stderr.
 */
char **split_binary_float(char *binary_float) {
  char **whole_float = (char **)malloc(3 * sizeof(char *));
  if (!whole_float) {
    perror("Memory allocation error.\n");
    return NULL;
  }

  char *current_bit = binary_float;

  // Extract sign bit
  whole_float[0] = (char *)malloc(2 * sizeof(char)); // sign bit and '\0'
  if (!whole_float[0]) {
    perror("Memory allocation error.\n");
    free(whole_float);
    return NULL;
  }

  whole_float[0][0] = binary_float[0];
  whole_float[0][1] = '\0';

  current_bit++;

  // Extract exponent bits
  whole_float[1] = (char *)malloc(9 * sizeof(char)); // exponent and '\0'
  if (!whole_float[1]) {
    perror("Memory allocation error.\n");
    free(whole_float[0]);
    free(whole_float);
    return NULL;
  }

  for (int i = 0; i < 8 && current_bit; i++, current_bit++) {
    whole_float[1][i] = *current_bit;
  }
  whole_float[1][8] = '\0';

  // Extract fraction bits
  whole_float[2] = (char *)malloc(24 * sizeof(char)); // fraction and '\0'
  if (!whole_float[2]) {
    perror("Memory allocation error.\n");
    free(whole_float[0]);
    free(whole_float[1]);
    free(whole_float);
    return NULL;
  }

  for (int i = 0; i < 23 && current_bit; i++, current_bit++) {
    whole_float[2][i] = *current_bit;
  }
  whole_float[2][23] = '\0';

  printf("\nBinary ---\nSign: %s Exponent: %s Fraction: %s\n", whole_float[0],
         whole_float[1], whole_float[2]);

  return whole_float;
}

/**
 * @brief Parses a binary string to a float value.
 *
 * Converts a string of '0's and '1's into its corresponding float value.
 * Handles both integer and fractional binary representations based on the
 * `is_fractional` flag.
 *
 * @param binary_string String of '0's and '1's representing a binary number.
 * @param is_fractional Integer flag to indicate fractional conversion.
 *                      If non-zero, the binary string is treated as a
 * fractional part (bits after binary point). If 0, the binary string is treated
 * as an integer.
 * @return float The float value represented by the binary string.
 * @note For fractional conversion, each bit is multiplied by decreasing powers
 * of 0.5 (1/2, 1/4, 1/8, ...).
 */
float parse_bits(char *binary_string, int is_fractional) {
  float acc = 0.0; // Accumulator
  if (!is_fractional) {
    while (*binary_string) {
      acc = acc * 2 + (*binary_string++ -
                       '0'); // Subtracting '0' converts the char to int
    }
  } else {
    float factor = 0.5;
    while (*binary_string) {
      acc += (*binary_string++ - '0') *
             factor; // Subtracting '0' converts the char to int

      factor /= 2;
    }
  }

  return acc;
}

/**
 * @brief Converts IEEE 754 single-precision float parts to a decimal double.
 *
 * Takes the sign, exponent, and fraction parts of a binary IEEE 754 float
 * (as strings) and converts them into a decimal `double` value.
 *
 * @param full_float Array of 3 strings representing: [sign, exponent, fraction]
 *                   from a split binary IEEE 754 float, typically the output of
 * `split_binary_float`.
 * @return double The decimal `double` representation of the IEEE float.
 *         Returns 0.0 and prints an error message to stderr if the exponent is
 * 255 (which in IEEE 754 standard represents NaN or Infinity).
 * @note Expects `full_float` to be the output of `split_binary_float`.
 *       Handles subnormal numbers (exponent is 0) according to IEEE 754
 * standard.
 */
double convert_ieee_float(char **full_float) {
  int exponent_size = 127; // Exponent uses 8 bits in floats
  float sign = parse_bits(full_float[0], 0);
  float exponent = parse_bits(full_float[1], 0);
  float fraction = parse_bits(full_float[2], 1);

  printf("\nDecimal ---\nSign: %.0f Exponent: %.0f Fraction: %f\n", sign,
         exponent, fraction);

  double sign_part = pow(-1.0, sign);

  double exp_part;
  if (exponent == 255) {
    perror("Exponent is 255\n");
    return 0;
  } else if (exponent == 0) {
    exp_part = pow(2.0, 1 - exponent_size); // Handle subnormals
  } else {
    exp_part = pow(2.0, (exponent - exponent_size));
  }

  double frac_part = (1.0 + fraction);

  return sign_part * exp_part * frac_part;
}
