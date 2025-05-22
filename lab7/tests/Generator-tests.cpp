#define CATCH_CONFIG_MAIN
#include "../../lib/catch2/catch.hpp"
#include "../generator/Generator.h"

namespace
{
struct Book
{
	std::string title;
	std::string author;
	std::vector<std::string> chapters;
};

struct BookChapter
{
	std::string bookTitle;
	std::string bookAuthor;
	std::string chapterTitle;
};

std::ostream& operator<<(std::ostream& os, const BookChapter& chapter)
{
	os << "Book: " << chapter.bookTitle
		<< " by " << chapter.bookAuthor
		<< ", Chapter: " << chapter.chapterTitle;
	return os;
}

Generator<BookChapter> ListBookChapters(const std::vector<Book>& books)
{
	for (const auto& [title, author, chapters] : books)
	{
		for (const auto& chapter : chapters)
		{
			BookChapter bookChapter{ title, author, chapter };
			co_yield std::move(bookChapter);
		}
	}
}
}

SCENARIO("generator tests with books")
{
	GIVEN("book chapters")
	{
		const std::vector<Book> books = {
			{ "The Great Gatsby", "F. Scott Fitzgerald", { "Chapter 1", "Chapter 2" } },
			{ "1984", "George Orwell", { "Chapter 1", "Chapter 2", "Chapter 3" } },
			{ "To Kill a Mockingbird", "Harper Lee", { "Chapter 1" } }
		};

		WHEN("list book chapters")
		{
			std::ostringstream output;
			for (const auto& chapter : ListBookChapters(books))
			{
				output << chapter << std::endl;
			}

			THEN("chapters was output")
			{
				CHECK(output.str() == ""
					"Book: The Great Gatsby by F. Scott Fitzgerald, Chapter: Chapter 1\n"
					"Book: The Great Gatsby by F. Scott Fitzgerald, Chapter: Chapter 2\n"
					"Book: 1984 by George Orwell, Chapter: Chapter 1\n"
					"Book: 1984 by George Orwell, Chapter: Chapter 2\n"
					"Book: 1984 by George Orwell, Chapter: Chapter 3\n"
					"Book: To Kill a Mockingbird by Harper Lee, Chapter: Chapter 1\n");
			}
		}
	}
}

namespace
{

Generator<int> FunctionWithException()
{
	co_yield 1;
	co_yield 2;
	throw std::runtime_error("Exception from coroutine");
	co_yield 3;
}

}

SCENARIO("generator tests with exceptions")
{
	WHEN("iterate function with exception")
	{
		std::ostringstream output;
		try
		{
			for (const auto& i : FunctionWithException())
			{
				output << i << " ";
			}
		}
		catch (const std::exception& e)
		{
			output << e.what();
		}

		THEN("exception was rethrown from coroutine")
		{
			CHECK(output.str() == "1 2 Exception from coroutine");
		}
	}
}