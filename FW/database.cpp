#include "FW/variant.h"
#include "FW/database.h"
#include <qsqlquery.h>
#include <qsqldatabase.h>
#include <qstring.h>
#include <qsqlrecord.h>
#include <qvariant.h>
#include <qdebug>

#define ADD_QUOTES(__VAR) (QString("\'").append(__VAR).append("\'"))

TypeDatabase::TypeDatabase( TypeVariant* parent ):
    TypeVariant( parent )
{

    QSqlDatabase::addDatabase( "QSQLITE" );
    // Note: Sqlite doesn't have commands to create or delete databases.
    // Anyway, qt creates the file if database doesn't exist yet.
}

TypeDatabase::~TypeDatabase()
{
    //void
}

bool TypeDatabase::OpenDatabase( QString file_name )
{
    QSqlDatabase::database().setDatabaseName( file_name );
    return QSqlDatabase::database().open();
}

void TypeDatabase::CloseDatabase()
{
    QSqlDatabase::database().close();
}

QString TypeDatabase::GetDatabaseName()
{
    return QSqlDatabase::database().databaseName();
}

void TypeDatabase::CreateTable( QString class_name, QStringList field_list )
{
    QString query_string;

    query_string
    .append( "CREATE TABLE " )
    .append( class_name )
    .append( " ( " );

    QString comma( "" );

    for( QString field : field_list )
    {

        query_string
        .append( comma )
        .append( field )
        .append( " varchar(255)" );

        comma = ",";
    }

    query_string.append( " )" );

    QSqlQuery query;
    query.exec( query_string );
}

void TypeDatabase::RemoveTable( QString class_name )
{
    QString query_string;

    query_string
    .append( "DROP TABLE " )
    .append( class_name );

    QSqlQuery query;
    query.exec( query_string );
}

bool TypeDatabase::TableExists( QString class_name )
{
    QString query_string;

    query_string
    .append( "SELECT COUNT(*) FROM information_schema.tables WHERE table_name=" )
    .append( ADD_QUOTES( class_name ) );

    QSqlQuery query;
    query.exec( query_string );
    query.next();

    return query.record().value( 0 ).toInt() > 0;
}

bool TypeDatabase::RecordExists( QString class_name, QString field, QString value )
{
    QString query_string;

    query_string
    .append( "SELECT COUNT(*) FROM " )
    .append( class_name )
    .append( " WHERE " )
    .append( field )
    .append( "=" )
    .append( ADD_QUOTES( value ) );

    QSqlQuery query;
    query.exec( query_string );
    query.next();

    return query.record().value( 0 ).toInt() > 0;
}


void TypeDatabase::AppendRecord( QString class_name, QStringList value_list )
{
    QString query_string;

    query_string
    .append( "INSERT INTO " )
    .append( class_name )
    .append( " VALUES ( " );

    QString comma( "" );

    for( QString value : value_list )
    {

        query_string
        .append( comma )
        .append( ADD_QUOTES( value ) );

        comma = ",";
    }

    query_string.append( " )" );

    QSqlQuery query;
    query.exec( query_string );
}

void TypeDatabase::RemoveRecord( QString class_name, QString field, QString value )
{
    QString query_string;

    query_string
    .append( "DELETE FROM " )
    .append( class_name )
    .append( " WHERE " )
    .append( field )
    .append( "=" )
    .append( ADD_QUOTES( value ) );

    QSqlQuery query;
    query.exec( query_string );

}

int TypeDatabase::RowCount( QString class_name )
{
    QString query_string;

    query_string
    .append( "SELECT COUNT(*) AS count FROM " )
    .append( class_name );

    QSqlQuery query;
    query.exec( query_string );
    query.next();

    return query.record().value( 0 ).toInt();
}

QStringList TypeDatabase::GetRecord( QString class_name, QString field, QString value )
{
    QString query_string;

    query_string
    .append( "SELECT * FROM " )
    .append( class_name )
    .append( " WHERE " )
    .append( field )
    .append( "=" )
    .append( ADD_QUOTES( value ) );

    QSqlQuery query;
    query.exec( query_string );
    query.next();

    QStringList value_list;

    int col_num = query.record().count();

    for( int n = 0; n < col_num; n++ )
    {
        value_list.append(
            query.record().value( n ).toString() );
    }

    return value_list;
}

