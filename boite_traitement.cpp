#include "boite_traitement.h"
#include "traitements/adaptateurs_qt/traitement_image.h"

#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPainter>

BoiteTraitement::BoiteTraitement(TraitementImage *traitement, QWidget *parent)
    : QWidget(parent),
    m_traitement(traitement)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet("background-color:#b10f3b; border-radius:15px;");

    m_etiquetteTitre = new QLabel(traitement ? traitement->nom() : "Traitement", this);
    m_etiquetteTitre->setAlignment(Qt::AlignCenter);
    m_etiquetteTitre->setStyleSheet("color:white; font-weight:bold;");

    m_etiquetteApercu = new QLabel(this);
    m_etiquetteApercu->setFixedSize(200, 140);
    m_etiquetteApercu->setStyleSheet("background:white; border-radius:8px;");
    m_etiquetteApercu->setScaledContents(true);

    m_boutonParametres = new QPushButton("Paramètres", this);
    m_boutonParametres->setStyleSheet(
        "QPushButton { background:white; color:#b10f3b; "
        "border-radius:10px; padding:4px 12px; }"
        "QPushButton:hover { background:#ffe6ee; }");

    auto *disposition = new QVBoxLayout(this);
    disposition->setContentsMargins(10, 10, 10, 10);
    disposition->setSpacing(8);
    disposition->addWidget(m_etiquetteTitre);
    disposition->addWidget(m_etiquetteApercu, 1);
    disposition->addWidget(m_boutonParametres);

    connect(m_boutonParametres, &QPushButton::clicked,
            this, &BoiteTraitement::surBoutonParametresClique);

    setFixedSize(230, 230);
}

void BoiteTraitement::definirImageApercu(const QImage &image)
{
    m_etiquetteApercu->setPixmap(QPixmap::fromImage(image));
}

void BoiteTraitement::surBoutonParametresClique()
{
    emit parametresDemandes(this);
}

void BoiteTraitement::mousePressEvent(QMouseEvent *evenement)
{
    if (evenement->button() == Qt::LeftButton) {
        m_enDeplacement = true;
        m_positionSourisLocal = evenement->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    QWidget::mousePressEvent(evenement);
}

void BoiteTraitement::mouseMoveEvent(QMouseEvent *evenement)
{
    if (m_enDeplacement) {
        QPoint nouvellePos = mapToParent(evenement->pos() - m_positionSourisLocal);

        // on garde la boite dans les limites du parent
        if (parentWidget()) {
            int x = qBound(0, nouvellePos.x(),
                           parentWidget()->width() - width());
            int y = qBound(0, nouvellePos.y(),
                           parentWidget()->height() - height());
            move(x, y);
            parentWidget()->update(); // pour redessiner les flèches
        } else {
            move(nouvellePos);
        }
    }
    QWidget::mouseMoveEvent(evenement);
}

void BoiteTraitement::mouseReleaseEvent(QMouseEvent *evenement)
{
    if (evenement->button() == Qt::LeftButton) {
        m_enDeplacement = false;
        unsetCursor();
    }
    QWidget::mouseReleaseEvent(evenement);
}

// petit ombrage optionnel
void BoiteTraitement::paintEvent(QPaintEvent *evenement)
{
    QWidget::paintEvent(evenement);
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.setPen(Qt::NoPen);
    p.setBrush(QColor(0, 0, 0, 40));
    QRect r = rect().adjusted(3, 3, -3, -1);
    p.drawRoundedRect(r, 15, 15);
}
