#include <gtest/gtest.h>

#include "infrastructure/manifests/ManifestParser.h"

using namespace reboard;
using infrastructure::ManifestParser;

TEST(ManifestParserTest, ParsesProcessManifest) {
    const ManifestParser parser;
    const auto app = parser.parse("koreader",
                                  "# KOReader manifest\n"
                                  "name=KOReader\n"
                                  "exec=/opt/koreader/koreader.sh --flag\n"
                                  "icon=/opt/icons/koreader.png\n"
                                  "dock=true\n");
    ASSERT_TRUE(app);
    EXPECT_EQ(app->id().value(), "koreader");
    EXPECT_EQ(app->name().value(), "KOReader");
    EXPECT_EQ(app->launchTarget().type(), domain::LaunchType::Process);
    ASSERT_EQ(app->launchTarget().argv().size(), 2u);
    EXPECT_EQ(app->launchTarget().argv()[0], "/opt/koreader/koreader.sh");
    EXPECT_EQ(app->iconPath(), "/opt/icons/koreader.png");
    EXPECT_TRUE(app->pinnedToDock());
}

TEST(ManifestParserTest, ParsesUnitManifest) {
    const ManifestParser parser;
    const auto app = parser.parse("xochitl", "name=Notebooks\nunit=xochitl\n");
    ASSERT_TRUE(app);
    EXPECT_EQ(app->launchTarget().type(), domain::LaunchType::SystemdUnit);
    EXPECT_EQ(app->launchTarget().unitName(), "xochitl");
    EXPECT_FALSE(app->pinnedToDock());
}

TEST(ManifestParserTest, IgnoresBlankLinesAndComments) {
    const ManifestParser parser;
    const auto app = parser.parse("app", "\n  \n# comment\nname=App\nexec=/bin/app\n\n");
    ASSERT_TRUE(app);
    EXPECT_EQ(app->name().value(), "App");
}

TEST(ManifestParserTest, RequiresName) {
    const ManifestParser parser;
    std::string error;
    EXPECT_FALSE(parser.parse("app", "exec=/bin/app\n", &error));
    EXPECT_NE(error.find("name"), std::string::npos);
}

TEST(ManifestParserTest, RequiresExactlyOneLaunchKey) {
    const ManifestParser parser;
    std::string error;
    EXPECT_FALSE(parser.parse("app", "name=App\n", &error));
    EXPECT_FALSE(parser.parse("app", "name=App\nexec=/bin/app\nunit=app\n", &error));
}

TEST(ManifestParserTest, RejectsMalformedLines) {
    const ManifestParser parser;
    std::string error;
    EXPECT_FALSE(parser.parse("app", "name=App\nnot a key value\nexec=/bin/app\n", &error));
    EXPECT_FALSE(parser.parse("app", "=value\nname=App\nexec=/bin/app\n", &error));
}

TEST(ManifestParserTest, RejectsInvalidDockValues) {
    const ManifestParser parser;
    std::string error;
    EXPECT_FALSE(parser.parse("app", "name=App\nexec=/bin/app\ndock=maybe\n", &error));
}

TEST(ManifestParserTest, AcceptsNumericDockValues) {
    const ManifestParser parser;
    const auto pinned = parser.parse("app", "name=App\nexec=/bin/app\ndock=1\n");
    ASSERT_TRUE(pinned);
    EXPECT_TRUE(pinned->pinnedToDock());
    const auto unpinned = parser.parse("app", "name=App\nexec=/bin/app\ndock=0\n");
    ASSERT_TRUE(unpinned);
    EXPECT_FALSE(unpinned->pinnedToDock());
}

TEST(ManifestParserTest, ReportsDomainValidationErrors) {
    const ManifestParser parser;
    std::string error;
    // Invalid id characters come from the file name.
    EXPECT_FALSE(parser.parse("bad id", "name=App\nexec=/bin/app\n", &error));
    EXPECT_FALSE(error.empty());
    // Invalid systemd unit name.
    EXPECT_FALSE(parser.parse("app", "name=App\nunit=bad unit\n", &error));
}

TEST(ManifestParserTest, TokenizesQuotedArguments) {
    const auto tokens =
        ManifestParser::tokenizeCommandLine("/bin/app \"argument with spaces\" --flag");
    ASSERT_TRUE(tokens);
    ASSERT_EQ(tokens->size(), 3u);
    EXPECT_EQ((*tokens)[1], "argument with spaces");
}

TEST(ManifestParserTest, TokenizerHandlesExtraWhitespace) {
    const auto tokens = ManifestParser::tokenizeCommandLine("  /bin/app   --a  \t --b ");
    ASSERT_TRUE(tokens);
    EXPECT_EQ(tokens->size(), 3u);
}

TEST(ManifestParserTest, TokenizerRejectsUnterminatedQuotes) {
    EXPECT_FALSE(ManifestParser::tokenizeCommandLine("/bin/app \"unterminated"));
}

TEST(ManifestParserTest, TokenizerSupportsEmptyQuotedArgument) {
    const auto tokens = ManifestParser::tokenizeCommandLine("/bin/app \"\"");
    ASSERT_TRUE(tokens);
    ASSERT_EQ(tokens->size(), 2u);
    EXPECT_EQ((*tokens)[1], "");
}
