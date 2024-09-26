#include <QApplication>
#include <QTextBrowser>
#include <QRegularExpression>

QString ansiToHtml(const QString &ansiString) {
    QString htmlString = ansiString;

    // 使用正则表达式匹配ANSI转义序列并转换为HTML
    QRegularExpression ansiRegex("\\x1B\\[([0-9;]+)m");
    htmlString.replace(ansiRegex, "<span style='ANSI_STYLE'>");

    // 恢复默认样式
    htmlString.replace("</span>", "</span>");

    return htmlString;
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 创建主窗口
    QTextBrowser textBrowser;
// \u001B]0;tong@X: ~/coins/coin/build\u0007\u001B[01;32mtong@X\u001B[00m:\u001B[01;34m~/coins/coin/build\u001B[00m$ ls
    // 示例ANSI字符串
    QString ansiString = "\u001B]0;tong@X: ~/coins/coin/build\u0007\u001B[01;32mtong@X\u001B[00m:\u001B[01;34m~/coins/coin/build\u001B[00m$ ls";

    // 转换为HTML
    QString htmlString = ansiToHtml(ansiString);

    // 设置HTML文本
    textBrowser.setHtml(htmlString);

    // 显示主窗口
    textBrowser.show();

    return a.exec();
}
