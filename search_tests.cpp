#include <gtest/gtest.h>

#include "search.h"
#include "tests/build_index.h"
#include "tests/clean_token.h"
#include "tests/find_query_matches.h"
#include "tests/gather_tokens.h"

//start cleanToken() tests**********

//Testing an empty string
TEST(CleanToken, EmptyString){
    string result = cleanToken("");
    //expect empty string output
    EXPECT_EQ(result, "");
    
}

//Testing no puntucation
TEST(CleanToken, noPuntuation){
    string result = cleanToken("hello");
    //expect no puntuation
    EXPECT_EQ(result, "hello");
}

//Testing lead punctuation
TEST(CleanToken, leadPunct){
    string result = cleanToken("!hello");
    //expect no punctuation
    EXPECT_EQ(result, "hello");
}

//Testing trailing punctuation
TEST(CleanToken, trailingPunct){
    string result = cleanToken("hello!");
    //expect no punctuation
    EXPECT_EQ(result, "hello");
}

//Test leading and trailing punctuation
TEST(CleanToken, leadTrailPunct){
    string result = cleanToken("!hello?");
    //expect no punctuation
    EXPECT_EQ(result, "hello");
}

//Test when there are no letters
TEST(CleanToken, noLetters){
    string result = cleanToken("123!?");
    //expect empty string
    EXPECT_EQ(result, "");
}

//Test uppercase letters
TEST(CleanToken, upperCase){
    string result = cleanToken("HELLO");
    //expect lowercase
    EXPECT_EQ(result, "hello");
}

//Test mixed characters and punctuation
TEST(CleanToken, mixed){
    string result = cleanToken("HelLO? TheRe!");
    //expect lowercase with removed punctuation
    EXPECT_EQ(result, "hello? there");
}

//Test punctuation in middle
TEST(CleanToken, punctMid){
    string result = cleanToken("hel!lo");
    //expect punctuation to remain in place
    EXPECT_EQ(result, "hel!lo");
}

//Test punctuation at beggining and middle
TEST(CleanToken, punctBegMid){
    string result = cleanToken("?hel!lo");
    //expect removed beggining punctuation, middle punct remains in place
    EXPECT_EQ(result, "hel!lo");
}

//Test punctuation at middle and end
TEST(CleanToken, punctMidEnd){
    string result = cleanToken("hel!lo?");
    //expect middle punct to remain, remove ending punctuation
    EXPECT_EQ(result, "hel!lo");
}

//end cleanToken() tests**********

//start gatherTokens() tests******

//Test basic
TEST(GatherTokens, basic){
    string text = "this is a test. it tests basic input.";
    set<string> expectedTokens = {"this", "is", "a", "test", "it", "tests", "basic", "input"};
    set<string> actualTokens = gatherTokens(text);
    ASSERT_EQ(actualTokens, expectedTokens);
}

//Test an empty string
TEST(GatherTokens, empty){
    string text = "";
    set<string> expectedTokens = {};
    set<string> actualTokens = gatherTokens(text);
    ASSERT_EQ(actualTokens, expectedTokens);
}

//Test leading spaces
TEST(GatherTokens, leadingSpaces){
    string text = "  hello there";
    set<string> expectedText = {"hello", "there"};
    set<string> result = gatherTokens(text);
    //expect white space to be removed
    ASSERT_EQ(result, expectedText);
}

//Test trailing spaces
TEST(GatherTokens, trailingSpaces){
    string text = "Hello there   ";
    set<string> expectedText = {"hello", "there"};
    set<string> result = gatherTokens(text);
    ASSERT_EQ(result, expectedText);
}

//test multiple spaces between
TEST(GatherTokens, spacesBetween){
    string text = "hello   there";
    set<string> expectedText = {"hello", "there"};
    set<string> result = gatherTokens(text);
    ASSERT_EQ(result, expectedText);
}

//end gatherTokens() tests******

//start buildIndex() tests******

//Tests an empty file
TEST(BuildIndex, emptySet){
    map<string, set<string>> index;
    string fileText = {};
    int result  = buildIndex(fileText, index);

    EXPECT_EQ(result, 0);
    EXPECT_TRUE(index.empty());
}

//Test invalid file name
TEST(BuildIndex, invalidFile){
    map<string, set<string>> index;
    string fileName = "invalid_file.txt";
    int result = buildIndex(fileName, index);
    //expect a non zero int
    EXPECT_EQ(result, 0);
    //expect an empty index
    EXPECT_TRUE(index.empty());
}

//Tests tiny.txt
TEST(BuildIndex, tinyFile){
    map<string, set<string>> expected = { {"eggs", {"www.shoppinglist.com"}},
                                     {"milk", {"www.shoppinglist.com"}}, 
                                     {"fish", {"www.shoppinglist.com", "www.dr.seuss.net"}},
                                     {"bread", {"www.shoppinglist.com"}},
                                     {"cheese", {"www.shoppinglist.com"}},
                                     {"red", {"www.rainbow.org", "www.dr.seuss.net"}},
                                     {"green", {"www.rainbow.org"}},
                                     {"orange", {"www.rainbow.org"}},
                                     {"yellow", {"www.rainbow.org"}},
                                     {"blue", {"www.rainbow.org", "www.dr.seuss.net"}},
                                     {"indigo", {"www.rainbow.org"}},
                                     {"violet", {"www.rainbow.org"}},
                                     {"one", {"www.dr.seuss.net"}},
                                     {"two", {"www.dr.seuss.net"}},
                                     {"i'm", {"www.bigbadwolf.com"}},
                                     {"not", {"www.bigbadwolf.com"}},
                                     {"trying", {"www.bigbadwolf.com"}},
                                     {"to", {"www.bigbadwolf.com"}},
                                     {"eat", {"www.bigbadwolf.com"}},
                                     {"you", {"www.bigbadwolf.com"}}};
    map<string, set<string>> index;
    string fileName = "tiny.txt";
    int result = buildIndex(fileName, index);

    EXPECT_EQ(expected, index);
    EXPECT_EQ(4, result);
}
//end buildIndex() tests******

//Start findQueryMatches() tests******
//tests if the first query is not in map
TEST(FindQueryMatches, FirstTermNotInMap){
    map<string, set<string>> index = {{"chevy", {"url1", "url2"}}, {"mazda", {"url2", "url3"}}};
    string query = "ford AND chevy";
    set<string> expected = {"url1", "url2"};
    set<string> actual = findQueryMatches(index, query);

    //expected to return an empty map since ford is false
    EXPECT_EQ(actual, expected);
}

// Test case for single term query
TEST(FindQueryMatches, SingleTermQuery) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}, {"term2", {"url2", "url3"}}};
    string query = "term1";
    set<string> expected = {"url1", "url2"};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}

// Test case for multiple term query connected with 'AND'
TEST(FindQueryMatches, MultipleTermsWithAnd) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}, {"term2", {"url2", "url3"}}};
    string query = "term1 AND term2";
    set<string> expected = {"url1", "url2", "url3"};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}

// Test case for multiple term query connected with 'OR'
TEST(FindQueryMatches, MultipleTermsWithOr) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}, {"term2", {"url2", "url3"}}};
    string query = "term1 OR term2";
    set<string> expected = {"url1", "url2", "url3"};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}

// Test case for multiple term query connected with 'NOT'
TEST(FindQueryMatches, MultipleTermsWithNot) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}, {"term2", {"url2", "url3"}}};
    string query = "term1 AND NOT term2";
    set<string> expected = {"url1", "url2", "url3"};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}

// Test case for a query where one term is not in the map
TEST(FindQueryMatches, TermNotInMap) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}};
    string query = "term1 AND term2";
    set<string> expected = {"url1", "url2"};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}

// Test case for a query where later term modified with '+' is not in the map
TEST(FindQueryMatches, LaterTermPlusNotInMap) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}};
    string query = "term1 AND term2 +term3";
    set<string> expected = {};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}

// Test case for a query where later term modified with '-' is not in the map
TEST(FindQueryMatches, LaterTermMinusNotInMap) {
    map<string, set<string>> index = {{"term1", {"url1", "url2"}}, {"term3", {"url1"}}};
    string query = "term1 AND term3 -term2";
    set<string> expected = {"url1", "url2"};
    set<string> actual = findQueryMatches(index, query);
    EXPECT_EQ(actual, expected);
}
//end findQueryMatches() tests*******

TEST(CleanToken, ExampleEmptyTest) {
    cout << "Hi! This shows up in the terminal." << endl;
}
