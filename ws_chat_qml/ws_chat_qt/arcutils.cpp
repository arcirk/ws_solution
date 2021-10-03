#include "arcutils.h"

ArcUtils::ArcUtils(QObject *parent) : QObject(parent)
{

}

QString ArcUtils::base64_decode(const QString &s)
{
    namespace bai = boost::archive::iterators;

    std::string source = s.toStdString();
    std::stringstream os;

    typedef bai::transform_width<bai::binary_from_base64<const char *>, 8, 6> base64_dec;

    unsigned int size = source.size();

    // Remove the padding characters, cf. https://svn.boost.org/trac/boost/ticket/5629
    if (size && source[size - 1] == '=') {
        --size;
        if (size && s[size - 1] == '=') --size;
    }
    if (size == 0) return QString::fromStdString(std::string());

    std::copy(base64_dec(s.data()), base64_dec(s.data() + size),
              std::ostream_iterator<char>(os));

    return QString::fromStdString(os.str());
}
