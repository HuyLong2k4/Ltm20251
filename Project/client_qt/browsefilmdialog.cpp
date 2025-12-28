#include "browsefilmdialog.h"
#include <QMessageBox>
#include <QGroupBox>

extern "C" {
    #include "../lib/socket/socket.h"
    #include "../lib/messages/message.h"
    #include "../client/headers/menu.h"
}

BrowseFilmDialog::BrowseFilmDialog(int sockfd, QWidget *parent)
    : QDialog(parent), sockfd(sockfd)
{
    setWindowTitle("Browse Films");
    setMinimumSize(700, 500);
    setupUI();
}

BrowseFilmDialog::~BrowseFilmDialog()
{
}

void BrowseFilmDialog::setupUI()
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    
    tabWidget = new QTabWidget(this);
    
    // ========== CATEGORY TAB ==========
    categoryTab = new QWidget();
    QVBoxLayout *categoryLayout = new QVBoxLayout(categoryTab);
    
    QLabel *categoryLabel = new QLabel("Select a Category:", categoryTab);
    categoryLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    categoryListWidget = new QListWidget(categoryTab);
    categoryListWidget->setMaximumHeight(150);
    
    browseCategoryBtn = new QPushButton("Browse Films", categoryTab);
    browseCategoryBtn->setMinimumHeight(35);
    
    QLabel *resultCategoryLabel = new QLabel("Films:", categoryTab);
    resultCategoryLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
    
    filmByCategoryListWidget = new QListWidget(categoryTab);
    
    categoryLayout->addWidget(categoryLabel);
    categoryLayout->addWidget(categoryListWidget);
    categoryLayout->addWidget(browseCategoryBtn);
    categoryLayout->addWidget(resultCategoryLabel);
    categoryLayout->addWidget(filmByCategoryListWidget);
    
    // Load categories
    char message[1024];
    sprintf(message, "SHOW_CATEGORIES\r\n");
    sendMessage(sockfd, message);
    
    while (true) {
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if (strcmp(message, "END") == 0) break;
        categoryListWidget->addItem(QString::fromUtf8(message));
    }
    
    // ========== CINEMA TAB ==========
    cinemaTab = new QWidget();
    QVBoxLayout *cinemaLayout = new QVBoxLayout(cinemaTab);
    
    QLabel *cinemaLabel = new QLabel("Select a Cinema:", cinemaTab);
    cinemaLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    cinemaListWidget = new QListWidget(cinemaTab);
    cinemaListWidget->setMaximumHeight(150);
    
    browseCinemaBtn = new QPushButton("Browse Films", cinemaTab);
    browseCinemaBtn->setMinimumHeight(35);
    
    QLabel *resultCinemaLabel = new QLabel("Films:", cinemaTab);
    resultCinemaLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
    
    filmByCinemaListWidget = new QListWidget(cinemaTab);
    
    cinemaLayout->addWidget(cinemaLabel);
    cinemaLayout->addWidget(cinemaListWidget);
    cinemaLayout->addWidget(browseCinemaBtn);
    cinemaLayout->addWidget(resultCinemaLabel);
    cinemaLayout->addWidget(filmByCinemaListWidget);
    
    // Load cinemas
    sprintf(message, "SHOW_CINEMAS\r\n");
    sendMessage(sockfd, message);
    
    while (true) {
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if (strcmp(message, "END") == 0) break;
        cinemaListWidget->addItem(QString::fromUtf8(message));
    }
    
    // ========== SHOWTIME TAB ==========
    showtimeTab = new QWidget();
    QVBoxLayout *showtimeLayout = new QVBoxLayout(showtimeTab);
    
    QLabel *showtimeLabel = new QLabel("Select a Showtime:", showtimeTab);
    showtimeLabel->setStyleSheet("font-weight: bold; font-size: 14px;");
    
    showtimeListWidget = new QListWidget(showtimeTab);
    showtimeListWidget->setMaximumHeight(150);
    
    browseShowtimeBtn = new QPushButton("Browse Films", showtimeTab);
    browseShowtimeBtn->setMinimumHeight(35);
    
    QLabel *resultShowtimeLabel = new QLabel("Films:", showtimeTab);
    resultShowtimeLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin-top: 10px;");
    
    filmByShowtimeListWidget = new QListWidget(showtimeTab);
    
    showtimeLayout->addWidget(showtimeLabel);
    showtimeLayout->addWidget(showtimeListWidget);
    showtimeLayout->addWidget(browseShowtimeBtn);
    showtimeLayout->addWidget(resultShowtimeLabel);
    showtimeLayout->addWidget(filmByShowtimeListWidget);
    
    // Load showtimes
    sprintf(message, "SHOW_PREMIERED_TIME\r\n");
    sendMessage(sockfd, message);
    
    while (true) {
        memset(message, 0, sizeof(message));
        recvMessage(sockfd, message);
        if (strcmp(message, "END") == 0) break;
        showtimeListWidget->addItem(QString::fromUtf8(message));
    }
    
    // Add tabs to tab widget
    tabWidget->addTab(categoryTab, "By Category");
    tabWidget->addTab(cinemaTab, "By Cinema");
    tabWidget->addTab(showtimeTab, "By Showtime");
    
    // Close button
    QPushButton *closeBtn = new QPushButton("Close", this);
    closeBtn->setMinimumHeight(35);
    
    mainLayout->addWidget(tabWidget);
    mainLayout->addWidget(closeBtn);
    
    // Connect signals
    connect(browseCategoryBtn, &QPushButton::clicked, this, &BrowseFilmDialog::onBrowseByCategory);
    connect(browseCinemaBtn, &QPushButton::clicked, this, &BrowseFilmDialog::onBrowseByCinema);
    connect(browseShowtimeBtn, &QPushButton::clicked, this, &BrowseFilmDialog::onBrowseByShowtime);
    connect(closeBtn, &QPushButton::clicked, this, &QDialog::accept);
}

void BrowseFilmDialog::onBrowseByCategory()
{
    QListWidgetItem *item = categoryListWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a category!");
        return;
    }
    
    QString selectedText = item->text();
    QStringList parts = selectedText.split(".");
    if (parts.isEmpty()) return;
    
    QString categoryId = parts[0].trimmed();
    
    char message[2048];
    char categoryIdStr[255];
    strcpy(categoryIdStr, categoryId.toUtf8().constData());
    
    makeBrowseFollowCategoryMessage(categoryIdStr, message);
    sendMessage(sockfd, message);
    
    int result = recvResult(sockfd);
    filmByCategoryListWidget->clear();
    
    if (result == BROWSE_CATEGORY_SUCCESS) {
        while (true) {
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if (strcmp(message, "END") == 0) break;
            filmByCategoryListWidget->addItem(QString::fromUtf8(message));
        }
    } else {
        filmByCategoryListWidget->addItem("No films found in this category!");
    }
}

void BrowseFilmDialog::onBrowseByCinema()
{
    QListWidgetItem *item = cinemaListWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a cinema!");
        return;
    }
    
    QString selectedText = item->text();
    QStringList parts = selectedText.split(".");
    if (parts.isEmpty()) return;
    
    QString cinemaId = parts[0].trimmed();
    
    char message[2048];
    char cinemaIdStr[255];
    strcpy(cinemaIdStr, cinemaId.toUtf8().constData());
    
    makeBrowseFollowCinemaMessage(cinemaIdStr, message);
    sendMessage(sockfd, message);
    
    int result = recvResult(sockfd);
    filmByCinemaListWidget->clear();
    
    if (result == BROWSE_THEATER_SUCCESS) {
        while (true) {
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if (strcmp(message, "END") == 0) break;
            filmByCinemaListWidget->addItem(QString::fromUtf8(message));
        }
    } else {
        filmByCinemaListWidget->addItem("No films found in this cinema!");
    }
}

void BrowseFilmDialog::onBrowseByShowtime()
{
    QListWidgetItem *item = showtimeListWidget->currentItem();
    if (!item) {
        QMessageBox::warning(this, "Warning", "Please select a showtime!");
        return;
    }
    
    QString selectedText = item->text();
    QStringList parts = selectedText.split(".");
    if (parts.isEmpty()) return;
    
    QString showtimeId = parts[0].trimmed();
    
    char message[2048];
    char showtimeIdStr[255];
    strcpy(showtimeIdStr, showtimeId.toUtf8().constData());
    
    makeBrowseFollowPremieredTimeMessage(showtimeIdStr, message);
    sendMessage(sockfd, message);
    
    int result = recvResult(sockfd);
    filmByShowtimeListWidget->clear();
    
    if (result == BROWSE_TIME_SUCCESS) {
        while (true) {
            memset(message, 0, sizeof(message));
            recvMessage(sockfd, message);
            if (strcmp(message, "END") == 0) break;
            filmByShowtimeListWidget->addItem(QString::fromUtf8(message));
        }
    } else {
        filmByShowtimeListWidget->addItem("No films found in this time slot!");
    }
}
