#pragma once
#include <QLine>
#include <QString>
#include <QVariant>
#include "../CommonDefs.h"
#include <QDataStream>
#include <QtGui/QApplication>

namespace Data
{
    class CWPInstrument
    {
        Q_DECLARE_TR_FUNCTIONS(CWPInstrument);

    public:
        enum CWPInstrumentType
        {
            WPInstrument_Null = 0,
            WPInstrument_Default = 1,
            WPInstrument_DisturbedLoad = 2,
            WPInstrument_FrictionConsidering = 3,
            WPInstrument_StressStrain = 4,
            WPInstrumentsTypesCount = 5
        };

        enum CWPInstrumentProperties
        {
            WPInstrProp_Type = 0,
            WPInstrProp_WPPointsMovement = 1,
            WPInstrProp_InsrMovementX = 2,
            WPInstrProp_InsrMovementY = 3,
            WPInstrProp_DisturbedLoad = 4,
            WPInstrProp_Friction = 5,
            WPInstrPropsCount = 6
        };

        CWPInstrument(QLineF line = QLineF(),
            CWPInstrumentType type = WPInstrument_Null);

        bool isNull() const;
        void setNull();

        CWPInstrumentType getType() const;
        void setType(CWPInstrumentType type);

        QString getTypeString() const;
        static QString getTypeString(CWPInstrumentType type);

        const QLineF &getLine() const;
        void setLine(const QLineF &line);

        QPointF getCenterPoint() const;

        QString getStringLong() const;
        QString getStringShort() const;

        // Returns [0.0f; 360.0f] degreeses in clock-wise direction
        double getYAsisAngle() const;

        bool operator ==(const CWPInstrument &other);

        // Properties
        //
        double getWorkpicePointsMovement() const;
        void setWorkpicePointsMovement(double fWorkpicePointsMovement);

        double getInstrumentMovementX() const;
        void setInstrumentMovementX(double fInstrumentMovementX);

        double getInstrumentMovementY() const;
        void setInstrumentMovementY(double fInstrumentMovementY);

        double getDisturbedLoad() const;
        void setDisturbedLoad(double fDisturbedLoad);

        double getFriction() const;
        void setFriction(double fFriction);

        // returns true if successfully
        bool getInstrumentPropertyValue(const CWPInstrumentProperties &propType,
            QVariant &value) const;
        // returns true if successfully
        bool setInstrumentPropertyValue(const CWPInstrumentProperties &propType,
            const QVariant &value);

        bool isPropertyAvaliableForCurrentType(
            const CWPInstrumentProperties &propType) const;

        void translatePointsIntoOutputOrder();

        // Output for the WPInstrument is INTRUMENT_LINE.
        // INTRUMENT_LINE is the string, for more details see the documentation.
        // 
        bool getOutputString(const CDoublesVector &movementsHV,
            const CDoublesVector &movementsC,
            const CDoublesVector &disturbedLoads,
            const CDoublesVector &frictions,
            QString &sOutput);

        bool isCanonical() const;
        bool isHorizontal() const;
        bool isVertical() const;
    private:
        QLineF m_line;
        CWPInstrumentType m_type;

        double m_fWorkpicePointsMovement;
        double m_fInstrumentMovementX;
        double m_fInstrumentMovementY;
        double m_fDisturbedLoad;
        double m_fFriction;
    private:
        void swapLinePoints();
    };
}

// Serialization/Deserealization fucntions
//
QDataStream & operator<< (QDataStream &out,
                          const Data::CWPInstrument &wpInstrument);
QDataStream & operator>> (QDataStream &in,
                          Data::CWPInstrument &wpInstrument);
