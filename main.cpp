#include <QApplication>
#include <QMainWindow>
#include <QPushButton>
#include <QLineEdit>
#include <QFileDialog>
#include <QLabel>
#include <QMessageBox>
#include <QFile>
#include <QByteArray>
#include <QImage>
#include <QDataStream>
#include <QRadioButton>
#include <QVBoxLayout>

enum Mode {
    ImageToBinary,
    BinaryToImage
};

Mode mode = ImageToBinary;
void convertImageToBinary(const QString &imagePath, const QString &outputFilePath) {
    QImage image(imagePath);
    if (image.isNull()) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось загрузить изображение.");
        return;
    }

    QFile outputFile(outputFilePath);
    if (!outputFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }

    QDataStream stream(&outputFile);

    // Сохраняем размеры изображения
    stream << image.width() << image.height();

    // Сохраняем данные пикселей
    for (int y = 0; y < image.height(); ++y) {
        for (int x = 0; x < image.width(); ++x) {
            QColor color = image.pixelColor(x, y);
            stream << color.red() << color.green() << color.blue();
        }
    }

    QMessageBox::information(nullptr, "Успех", "Изображение успешно преобразовано в двоичный код и сохранено в файле.");
}

void convertBinaryToImage(const QString &binaryFilePath, const QString &outputFolderPath) {
    QFile binaryFile(binaryFilePath);
    if (!binaryFile.open(QIODevice::ReadOnly)) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось открыть файл с двоичным кодом.");
        return;
    }

    QDataStream stream(&binaryFile);

    int width, height;
    stream >> width >> height; // Читаем размеры изображения

    QImage image(width, height, QImage::Format_RGB32);
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            int red, green, blue;
            stream >> red >> green >> blue;
            image.setPixelColor(x, y, QColor(red, green, blue));
        }
    }

    QString outputFilePath = outputFolderPath + "/output_image.png";
    if (!image.save(outputFilePath)) {
        QMessageBox::critical(nullptr, "Ошибка", "Не удалось сохранить изображение.");
        return;
    }

    QMessageBox::information(nullptr, "Успех", "Двоичный код успешно преобразован в изображение и сохранен.");
}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QMainWindow window;
    window.setWindowTitle("Image Converter");

    QLabel *inputLabel = new QLabel("Выберите файл:");
    QLineEdit *inputFileLineEdit = new QLineEdit;
    QPushButton *inputFileButton = new QPushButton("Обзор...");

    QLabel *outputLabel = new QLabel("Выберите папку:");
    QLineEdit *outputFileLineEdit = new QLineEdit;
    QPushButton *outputFileButton = new QPushButton("Обзор...");

    QPushButton *convertButton = new QPushButton("Преобразовать");

    //переключатели режимов
    QRadioButton *imageToBinaryRadioButton = new QRadioButton("Изображение в двоичный код");
    QRadioButton *binaryToImageRadioButton = new QRadioButton("Двоичный код в изображение");

    //"Обзор" файла
    QObject::connect(inputFileButton, &QPushButton::clicked, [&] {
        QString filePath = QFileDialog::getOpenFileName(nullptr, "Выберите файл", QString(), "Все файлы (*)");
        inputFileLineEdit->setText(filePath);
    });

    //"Обзор" папки
    QObject::connect(outputFileButton, &QPushButton::clicked, [&] {
        QString folderPath = QFileDialog::getExistingDirectory(nullptr, "Выберите папку", QString());
        outputFileLineEdit->setText(folderPath);
    });

    //"Преобразовать"
    QObject::connect(convertButton, &QPushButton::clicked, [&] {
        if (mode == ImageToBinary) {
            QString imagePath = inputFileLineEdit->text();
            QString outputFilePath = outputFileLineEdit->text() + "/binary_output.txt";
            convertImageToBinary(imagePath, outputFilePath);
        } else if (mode == BinaryToImage) {
            QString binaryFilePath = inputFileLineEdit->text();
            QString outputFolderPath = outputFileLineEdit->text();
            convertBinaryToImage(binaryFilePath, outputFolderPath);
        }
    });

    // Обработчик события для переключателей режимов
    QObject::connect(imageToBinaryRadioButton, &QRadioButton::clicked, [&] {
        mode = ImageToBinary;
    });

    QObject::connect(binaryToImageRadioButton, &QRadioButton::clicked, [&] {
        mode = BinaryToImage;
    });

    //макет для размещения виджетов
    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(inputLabel);
    layout->addWidget(inputFileLineEdit);
    layout->addWidget(inputFileButton);
    layout->addWidget(outputLabel);
    layout->addWidget(outputFileLineEdit);
    layout->addWidget(outputFileButton);
    layout->addWidget(imageToBinaryRadioButton);
    layout->addWidget(binaryToImageRadioButton);
    layout->addWidget(convertButton);

    //центральный виджет и устанавливаем макет
    QWidget *centralWidget = new QWidget;
    centralWidget->setLayout(layout);
    window.setCentralWidget(centralWidget);

    window.show();

    return app.exec();
}
