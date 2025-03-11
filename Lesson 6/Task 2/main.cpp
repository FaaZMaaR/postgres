#include <iostream>

#include <Wt/Dbo/Dbo.h>
#include <Wt/Dbo/backend/Postgres.h>

#include <Windows.h>
#pragma execution_character_set("utf-8")

class Book;
class Stock;
class Sale;

class Publisher {
public:
    std::string name = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToOne, "publisher");
    }
};

class Shop {
public:
    std::string name = "";
    Wt::Dbo::collection<Wt::Dbo::ptr<Book>> books;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, name, "name");
        Wt::Dbo::hasMany(a, books, Wt::Dbo::ManyToMany, "stock");
    }
};

class Book {
public:
    std::string title = "";
    Wt::Dbo::ptr<Publisher> publisher;
    Wt::Dbo::collection<Wt::Dbo::ptr<Shop>> shops;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, title, "title");
        Wt::Dbo::belongsTo(a, publisher, "publisher");
        Wt::Dbo::hasMany(a, shops, Wt::Dbo::ManyToMany, "stock");
    }
};

class Stock {
public:
    int count = 0;
    Wt::Dbo::ptr<Book> book;
    Wt::Dbo::ptr<Shop> shop;
    Wt::Dbo::collection<Wt::Dbo::ptr<Sale>> sales;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, count, "count");
        Wt::Dbo::belongsTo(a, book, "book");
        Wt::Dbo::belongsTo(a, shop, "shop");
        Wt::Dbo::hasMany(a, sales, Wt::Dbo::ManyToOne, "stock");
    }
};

class Sale {
public:
    int price = 0;
    std::string date_sale;
    Wt::Dbo::ptr<Stock> stock;
    int count = 0;

    template<class Action>
    void persist(Action& a) {
        Wt::Dbo::field(a, price, "price");
        Wt::Dbo::field(a, date_sale, "date_sale");
        Wt::Dbo::belongsTo(a, stock, "stock");
        Wt::Dbo::field(a, count, "count");
    }
};

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    try {
        std::string dbname, password;
        std::cout << "Введите имя БД: ";
        std::cin >> dbname;
        std::cout << "Введите пароль: ";
        std::cin >> password;
        std::string connectionString =
            "host=localhost"
            " port=5432"
            " dbname=" + dbname +
            " user=postgres"
            " password=" + password;

        auto postgres = std::make_unique<Wt::Dbo::backend::Postgres>(connectionString);

        Wt::Dbo::Session session;
        session.setConnection(std::move(postgres));
        session.mapClass<Publisher>("publisher");
        session.mapClass<Shop>("shop");
        session.mapClass<Book>("book");
        session.mapClass<Stock>("stock");
        session.mapClass<Sale>("sale");

        char answer;

        std::cout << "Нужно ли создать таблицы (y/n)?: ";
        std::cin >> answer;
        if (answer == 'y') {
            session.createTables();
        }

        std::cout << "Нужно ли заполнить таблицы данными (y/n)?: ";
        std::cin >> answer;
        if (answer == 'y') {
            Wt::Dbo::Transaction transaction1{ session };

            std::unique_ptr<Publisher> publisher1{ new Publisher };
            std::unique_ptr<Publisher> publisher2{ new Publisher };
            std::unique_ptr<Publisher> publisher3{ new Publisher };

            std::unique_ptr<Book> book1{ new Book };
            std::unique_ptr<Book> book2{ new Book };
            std::unique_ptr<Book> book3{ new Book };
            std::unique_ptr<Book> book4{ new Book };
            std::unique_ptr<Book> book5{ new Book };
            std::unique_ptr<Book> book6{ new Book };
            std::unique_ptr<Book> book7{ new Book };

            std::unique_ptr<Shop> shop1{ new Shop };
            std::unique_ptr<Shop> shop2{ new Shop };

            std::unique_ptr<Stock> stock1{ new Stock };
            std::unique_ptr<Stock> stock2{ new Stock };
            std::unique_ptr<Stock> stock3{ new Stock };
            std::unique_ptr<Stock> stock4{ new Stock };
            std::unique_ptr<Stock> stock5{ new Stock };
            std::unique_ptr<Stock> stock6{ new Stock };
            std::unique_ptr<Stock> stock7{ new Stock };
            std::unique_ptr<Stock> stock8{ new Stock };

            std::unique_ptr<Sale> sale1{ new Sale };
            std::unique_ptr<Sale> sale2{ new Sale };
            std::unique_ptr<Sale> sale3{ new Sale };
            std::unique_ptr<Sale> sale4{ new Sale };

            Wt::Dbo::ptr<Publisher> publisherPtr;
            Wt::Dbo::ptr<Book> bookPtr;
            Wt::Dbo::ptr<Shop> shopPtr;
            Wt::Dbo::ptr<Stock> stockPtr;

            publisher1->name = "Я программист";
            publisher2->name = "Наука и техника";
            publisher3->name = "Охота и рыбалка";

            shop1->name = "Мир увлечений";
            shop2->name = "Технический прогресс";

            publisherPtr = session.add(std::move(publisher1));
            book1->title = "Основы C++";
            book1->publisher = publisherPtr;
            book2->title = "Алгоритмы и структуры данных";
            book2->publisher = publisherPtr;
            book3->title = "Современная графика в играх";
            book3->publisher = publisherPtr;

            publisherPtr = session.add(std::move(publisher2));
            book4->title = "Теория струн";
            book4->publisher = publisherPtr;
            book5->title = "Нанотехнологии";
            book5->publisher = publisherPtr;

            publisherPtr = session.add(std::move(publisher3));
            book6->title = "Рыболовные снасти";
            book6->publisher = publisherPtr;
            book7->title = "Охота на кабана";
            book7->publisher = publisherPtr;

            bookPtr = session.add(std::move(book1));
            shopPtr = session.add(std::move(shop1));
            stock1->book = bookPtr;
            stock1->shop = shopPtr;
            stock1->count = 5;

            bookPtr = session.add(std::move(book2));
            stock2->book = bookPtr;
            stock2->shop = shopPtr;
            stock2->count = 4;
            session.add(std::move(stock2));

            bookPtr = session.add(std::move(book6));
            stock3->book = bookPtr;
            stock3->shop = shopPtr;
            stock3->count = 6;
            session.add(std::move(stock3));

            bookPtr = session.add(std::move(book7));
            stock4->book = bookPtr;
            stock4->shop = shopPtr;
            stock4->count = 2;
            session.add(std::move(stock4));

            bookPtr = session.add(std::move(book3));
            stock5->book = bookPtr;
            stock5->shop = shopPtr;
            stock5->count = 7;

            shopPtr = session.add(std::move(shop2));
            stock6->book = bookPtr;
            stock6->shop = shopPtr;
            stock6->count = 8;

            bookPtr = session.add(std::move(book4));
            stock7->book = bookPtr;
            stock7->shop = shopPtr;
            stock7->count = 5;
            session.add(std::move(stock7));

            bookPtr = session.add(std::move(book5));
            stock8->book = bookPtr;
            stock8->shop = shopPtr;
            stock8->count = 6;

            stockPtr = session.add(std::move(stock1));
            sale1->price = 850;
            sale1->date_sale = "2025-02-03";
            sale1->stock = stockPtr;
            sale1->count = 2;
            session.add(std::move(sale1));

            stockPtr = session.add(std::move(stock5));
            sale2->price = 1260;
            sale2->date_sale = "2025-02-15";
            sale2->stock = stockPtr;
            sale2->count = 3;
            session.add(std::move(sale2));

            stockPtr = session.add(std::move(stock6));
            sale3->price = 770;
            sale3->date_sale = "2025-01-24";
            sale3->stock = stockPtr;
            sale3->count = 4;
            session.add(std::move(sale3));

            stockPtr = session.add(std::move(stock8));
            sale4->price = 2100;
            sale4->date_sale = "2025-03-02";
            sale4->stock = stockPtr;
            sale4->count = 4;
            session.add(std::move(sale4));

            transaction1.commit();
        }        

        Wt::Dbo::Transaction transaction2{ session };

        Wt::Dbo::collection<Wt::Dbo::ptr<Publisher>> publishers = session.find<Publisher>();
        std::cout << "\nИздатели:" << std::endl;
        for (const Wt::Dbo::ptr<Publisher>& pblr : publishers) {
            std::cout << pblr.id() << " - " << pblr->name << std::endl;
        }

        int publisherID;
        std::cout << "\nВведите id издателя: ";
        std::cin >> publisherID;

        std::cout << "\nСписок магазинов, в которых продают книги искомого издателя:\n";

        Wt::Dbo::collection<Wt::Dbo::ptr<Shop>> shops = session.find<Shop>();
        for (const Wt::Dbo::ptr<Shop>& shop : shops) {
            using ResultType = std::tuple<Wt::Dbo::ptr<Shop>, Wt::Dbo::ptr<Stock>, Wt::Dbo::ptr<Book>, Wt::Dbo::ptr<Publisher>>;
            ResultType results = session.query<ResultType>("SELECT s, st, b, p FROM \"shop\" s").join<Stock>("st", "s.id = st.shop_id").join<Book>("b","st.book_id = b.id").join<Publisher>("p","b.publisher_id = p.id").where("s.id = ?").bind(shop.id()).where("p.id = ?").bind(publisherID).limit(1);
            if (std::get<0>(results)) {
                std::cout << (std::get<0>(results))->name << std::endl;
            }            
        }
        transaction2.commit();

    }
    catch (const std::exception& err) {
        std::cout << err.what() << std::endl;
    }
}