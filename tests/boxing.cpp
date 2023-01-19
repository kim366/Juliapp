#include "julia.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_exception.hpp>

TEST_CASE("Boxing")
{
    SECTION("Bool True")
    {
        const auto input = true;
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_bool(subject.raw()));
        REQUIRE(jl_unbox_bool(subject.raw()) == expected);
    }

    SECTION("Bool False")
    {
        const auto input = false;
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_bool(subject.raw()));
        REQUIRE(jl_unbox_bool(subject.raw()) == expected);
    }

    SECTION("Int8")
    {
        const auto input = std::int8_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_int8(subject.raw()));
        REQUIRE(jl_unbox_int8(subject.raw()) == expected);
    }

    SECTION("Int16")
    {
        const auto input = std::int16_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_int16(subject.raw()));
        REQUIRE(jl_unbox_int16(subject.raw()) == expected);
    }

    SECTION("Int32")
    {
        const auto input = std::int32_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_int32(subject.raw()));
        REQUIRE(jl_unbox_int32(subject.raw()) == expected);
    }

    SECTION("Int64")
    {
        const auto input = std::int64_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_int64(subject.raw()));
        REQUIRE(jl_unbox_int64(subject.raw()) == expected);
    }

    SECTION("UInt8")
    {
        const auto input = std::uint8_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_uint8(subject.raw()));
        REQUIRE(jl_unbox_uint8(subject.raw()) == expected);
    }

    SECTION("UInt16")
    {
        const auto input = std::uint16_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_uint16(subject.raw()));
        REQUIRE(jl_unbox_uint16(subject.raw()) == expected);
    }

    SECTION("UInt32")
    {
        const auto input = std::uint32_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_uint32(subject.raw()));
        REQUIRE(jl_unbox_uint32(subject.raw()) == expected);
    }

    SECTION("UInt64")
    {
        const auto input = std::uint64_t{19};
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_is_uint64(subject.raw()));
        REQUIRE(jl_unbox_uint64(subject.raw()) == expected);
    }

    SECTION("Float32")
    {
        const auto input = 19.f;
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_typeis(subject.raw(), jl_float32_type));
        REQUIRE(jl_unbox_float32(subject.raw()) == expected);
    }

    SECTION("Float64")
    {
        const auto input = 19.;
        const auto expected = input;

        const auto subject = jl::value{input};

        REQUIRE(jl_typeis(subject.raw(), jl_float64_type));
        REQUIRE(jl_unbox_float64(subject.raw()) == expected);
    }
}

TEST_CASE("Unboxing")
{
    SECTION("Float32 successful")
    {
        const auto input = 323.89f;
        const auto expected = input;
        auto* raw = jl_box_float32(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<float>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Float32 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<float>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Float32 but value contains Float64"));
    }

    SECTION("Float64 successful")
    {
        const auto input = 323.89;
        const auto expected = input;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<double>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Float64 unsuccessful")
    {
        const auto input = true;
        auto* raw = jl_box_bool(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<double>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Float64 but value contains Bool"));
    }

    SECTION("Bool true successful")
    {
        const auto input = true;
        const auto expected = input;
        auto* raw = jl_box_bool(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<bool>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Bool true successful")
    {
        const auto input = false;
        const auto expected = input;
        auto* raw = jl_box_bool(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<bool>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Bool unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<bool>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Bool but value contains Float64"));
    }

    SECTION("Int8 successful")
    {
        const auto input = std::int8_t{91};
        const auto expected = input;
        auto* raw = jl_box_int8(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::int8_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Int8 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::int8_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Int8 but value contains Float64"));
    }

    SECTION("Int16 successful")
    {
        const auto input = std::int16_t{91};
        const auto expected = input;
        auto* raw = jl_box_int16(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::int16_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Int16 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::int16_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Int16 but value contains Float64"));
    }

    SECTION("Int32 successful")
    {
        const auto input = std::int32_t{91};
        const auto expected = input;
        auto* raw = jl_box_int32(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::int32_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Int32 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::int32_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Int32 but value contains Float64"));
    }

    SECTION("Int64 successful")
    {
        const auto input = std::int64_t{91};
        const auto expected = input;
        auto* raw = jl_box_int64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::int64_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("Int64 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::int64_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected Int64 but value contains Float64"));
    }

    SECTION("UInt8 successful")
    {
        const auto input = std::uint8_t{91};
        const auto expected = input;
        auto* raw = jl_box_uint8(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::uint8_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("UInt8 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::uint8_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected UInt8 but value contains Float64"));
    }

    SECTION("UInt16 successful")
    {
        const auto input = std::uint16_t{91};
        const auto expected = input;
        auto* raw = jl_box_uint16(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::uint16_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("UInt16 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::uint16_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected UInt16 but value contains Float64"));
    }

    SECTION("UInt32 successful")
    {
        const auto input = std::uint32_t{91};
        const auto expected = input;
        auto* raw = jl_box_uint32(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::uint32_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("UInt32 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::uint32_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected UInt32 but value contains Float64"));
    }

    SECTION("UInt64 successful")
    {
        const auto input = std::uint64_t{91};
        const auto expected = input;
        auto* raw = jl_box_uint64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        const auto result = jl::value_cast<std::uint64_t>(subject);

        REQUIRE(result == expected);
    }

    SECTION("UInt64 unsuccessful")
    {
        const auto input = 323.89;
        auto* raw = jl_box_float64(input);
        const auto subject = jl::value{jl::from_raw, raw};

        REQUIRE_THROWS_MATCHES(jl::value_cast<std::uint64_t>(subject), jl::bad_value_cast, Catch::Matchers::Message("Expected UInt64 but value contains Float64"));
    }
}
