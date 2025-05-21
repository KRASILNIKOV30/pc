#include <iostream>
#include <vector>
#include <string>
#include "Generator.h"

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
			co_yield BookChapter{ title, author, chapter };
		}
	}
}

int main()
{
	const std::vector<Book> books = {
		{ "The Great Gatsby", "F. Scott Fitzgerald", { "Chapter 1", "Chapter 2" } },
		{ "1984", "George Orwell", { "Chapter 1", "Chapter 2", "Chapter 3" } },
		{ "To Kill a Mockingbird", "Harper Lee", { "Chapter 1" } }
	};

	for (const auto& chapter : ListBookChapters(books))
	{
		std::cout << chapter << std::endl;
	}
}