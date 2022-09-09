#include <gtest/gtest.h>
#include <common/encryption.h>

TEST(common_encryption, tea32)
{
    const char* text[] = {
        "Hello World",
        "采用TEA-32(Tiny Encryption Algorithm)的方式加密数据.",
        "The mouse pointer is considered to be hovering when it stays within a specified rectangle for a specified period of time. Call SystemParametersInfo. and use the values SPI_GETMOUSEHOVERWIDTH, SPI_GETMOUSEHOVERHEIGHT, and SPI_GETMOUSEHOVERTIME to retrieve the size of the rectangle and the time.",
        nullptr
    };

    const char* password[] = {
        "password",
        "A74ED44095B6464989E15BDD2D491158",
        "",
        nullptr
    };

    for (int i = 0; text[i] != nullptr && password[i] != nullptr; ++i)
    {
        util::bytedata orgin = text[i];
        util::bytedata bytes = orgin;
        util::encryption::encrypt_with_tea32(bytes, password[i]);

        EXPECT_NE(orgin, bytes);
        EXPECT_TRUE(orgin.size() < bytes.size());

        util::encryption::decrypt_with_tea32(bytes, password[i]);
        EXPECT_EQ(orgin, bytes);
    }
}
