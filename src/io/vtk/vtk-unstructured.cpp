// Copyright (c) 2005-2014 Code Synthesis Tools CC
//
// This program was generated by CodeSynthesis XSD, an XML Schema to
// C++ data binding compiler.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License version 2 as
// published by the Free Software Foundation.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
//
// In addition, as a special exception, Code Synthesis Tools CC gives
// permission to link this program with the Xerces-C++ library (or with
// modified versions of Xerces-C++ that use the same license as Xerces-C++),
// and distribute linked combinations including the two. You must obey
// the GNU General Public License version 2 in all respects for all of
// the code used other than Xerces-C++. If you modify this copy of the
// program, you may extend this exception to your version of the program,
// but you are not obligated to do so. If you do not wish to do so, delete
// this exception statement from your version.
//
// Furthermore, Code Synthesis Tools CC makes a special exception for
// the Free/Libre and Open Source Software (FLOSS) which is described
// in the accompanying FLOSSE file.
//

// Begin prologue.
//
//
// End prologue.

#include "vtk-unstructured.h"

#include <xsd/cxx/pre.hxx>


// DataArrayList_t
//

DataArrayList_t::
DataArrayList_t ()
: ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal > (this)
{
}

DataArrayList_t::
DataArrayList_t (size_type n, const ::xml_schema::decimal& x)
: ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal > (n, x, this)
{
}

DataArrayList_t::
DataArrayList_t (const DataArrayList_t& o,
                 ::xml_schema::flags f,
                 ::xml_schema::container* c)
: ::xml_schema::simple_type (o, f, c),
  ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal > (o, f, this)
{
}

// DataArray_t
// 

const DataArray_t::type_type& DataArray_t::
type () const
{
  return this->type_.get ();
}

DataArray_t::type_type& DataArray_t::
type ()
{
  return this->type_.get ();
}

void DataArray_t::
type (const type_type& x)
{
  this->type_.set (x);
}

void DataArray_t::
type (::std::auto_ptr< type_type > x)
{
  this->type_.set (x);
}

const DataArray_t::Name_type& DataArray_t::
Name () const
{
  return this->Name_.get ();
}

DataArray_t::Name_type& DataArray_t::
Name ()
{
  return this->Name_.get ();
}

void DataArray_t::
Name (const Name_type& x)
{
  this->Name_.set (x);
}

void DataArray_t::
Name (::std::auto_ptr< Name_type > x)
{
  this->Name_.set (x);
}

const DataArray_t::NumberOfComponents_type& DataArray_t::
NumberOfComponents () const
{
  return this->NumberOfComponents_.get ();
}

DataArray_t::NumberOfComponents_type& DataArray_t::
NumberOfComponents ()
{
  return this->NumberOfComponents_.get ();
}

void DataArray_t::
NumberOfComponents (const NumberOfComponents_type& x)
{
  this->NumberOfComponents_.set (x);
}

const DataArray_t::format_type& DataArray_t::
format () const
{
  return this->format_.get ();
}

const DataArray_t::format_type& DataArray_t::
format_default_value ()
{
  return format_default_value_;
}

const DataArray_t::offset_optional& DataArray_t::
offset () const
{
  return this->offset_;
}

DataArray_t::offset_optional& DataArray_t::
offset ()
{
  return this->offset_;
}

void DataArray_t::
offset (const offset_type& x)
{
  this->offset_.set (x);
}

void DataArray_t::
offset (const offset_optional& x)
{
  this->offset_ = x;
}


// PieceUnstructuredGrid_t
// 

const PieceUnstructuredGrid_t::PointData_type& PieceUnstructuredGrid_t::
PointData () const
{
  return this->PointData_.get ();
}

PieceUnstructuredGrid_t::PointData_type& PieceUnstructuredGrid_t::
PointData ()
{
  return this->PointData_.get ();
}

void PieceUnstructuredGrid_t::
PointData (const PointData_type& x)
{
  this->PointData_.set (x);
}

void PieceUnstructuredGrid_t::
PointData (::std::auto_ptr< PointData_type > x)
{
  this->PointData_.set (x);
}

const PieceUnstructuredGrid_t::CellData_type& PieceUnstructuredGrid_t::
CellData () const
{
  return this->CellData_.get ();
}

PieceUnstructuredGrid_t::CellData_type& PieceUnstructuredGrid_t::
CellData ()
{
  return this->CellData_.get ();
}

void PieceUnstructuredGrid_t::
CellData (const CellData_type& x)
{
  this->CellData_.set (x);
}

void PieceUnstructuredGrid_t::
CellData (::std::auto_ptr< CellData_type > x)
{
  this->CellData_.set (x);
}

const PieceUnstructuredGrid_t::Points_type& PieceUnstructuredGrid_t::
Points () const
{
  return this->Points_.get ();
}

PieceUnstructuredGrid_t::Points_type& PieceUnstructuredGrid_t::
Points ()
{
  return this->Points_.get ();
}

void PieceUnstructuredGrid_t::
Points (const Points_type& x)
{
  this->Points_.set (x);
}

void PieceUnstructuredGrid_t::
Points (::std::auto_ptr< Points_type > x)
{
  this->Points_.set (x);
}

const PieceUnstructuredGrid_t::Cells_type& PieceUnstructuredGrid_t::
Cells () const
{
  return this->Cells_.get ();
}

PieceUnstructuredGrid_t::Cells_type& PieceUnstructuredGrid_t::
Cells ()
{
  return this->Cells_.get ();
}

void PieceUnstructuredGrid_t::
Cells (const Cells_type& x)
{
  this->Cells_.set (x);
}

void PieceUnstructuredGrid_t::
Cells (::std::auto_ptr< Cells_type > x)
{
  this->Cells_.set (x);
}

const PieceUnstructuredGrid_t::NumberOfPoints_type& PieceUnstructuredGrid_t::
NumberOfPoints () const
{
  return this->NumberOfPoints_.get ();
}

PieceUnstructuredGrid_t::NumberOfPoints_type& PieceUnstructuredGrid_t::
NumberOfPoints ()
{
  return this->NumberOfPoints_.get ();
}

void PieceUnstructuredGrid_t::
NumberOfPoints (const NumberOfPoints_type& x)
{
  this->NumberOfPoints_.set (x);
}

const PieceUnstructuredGrid_t::NumberOfCells_type& PieceUnstructuredGrid_t::
NumberOfCells () const
{
  return this->NumberOfCells_.get ();
}

PieceUnstructuredGrid_t::NumberOfCells_type& PieceUnstructuredGrid_t::
NumberOfCells ()
{
  return this->NumberOfCells_.get ();
}

void PieceUnstructuredGrid_t::
NumberOfCells (const NumberOfCells_type& x)
{
  this->NumberOfCells_.set (x);
}


// UnstructuredGrid_t
// 

const UnstructuredGrid_t::Piece_type& UnstructuredGrid_t::
Piece () const
{
  return this->Piece_.get ();
}

UnstructuredGrid_t::Piece_type& UnstructuredGrid_t::
Piece ()
{
  return this->Piece_.get ();
}

void UnstructuredGrid_t::
Piece (const Piece_type& x)
{
  this->Piece_.set (x);
}

void UnstructuredGrid_t::
Piece (::std::auto_ptr< Piece_type > x)
{
  this->Piece_.set (x);
}


// type
// 

type::
type (value v)
: ::xml_schema::string (_xsd_type_literals_[v])
{
}

type::
type (const char* v)
: ::xml_schema::string (v)
{
}

type::
type (const ::std::string& v)
: ::xml_schema::string (v)
{
}

type::
type (const ::xml_schema::string& v)
: ::xml_schema::string (v)
{
}

type::
type (const type& v,
      ::xml_schema::flags f,
      ::xml_schema::container* c)
: ::xml_schema::string (v, f, c)
{
}

type& type::
operator= (value v)
{
  static_cast< ::xml_schema::string& > (*this) = 
  ::xml_schema::string (_xsd_type_literals_[v]);

  return *this;
}


// PointData
// 

const PointData::DataArray_sequence& PointData::
DataArray () const
{
  return this->DataArray_;
}

PointData::DataArray_sequence& PointData::
DataArray ()
{
  return this->DataArray_;
}

void PointData::
DataArray (const DataArray_sequence& s)
{
  this->DataArray_ = s;
}


// CellData
// 

const CellData::DataArray_sequence& CellData::
DataArray () const
{
  return this->DataArray_;
}

CellData::DataArray_sequence& CellData::
DataArray ()
{
  return this->DataArray_;
}

void CellData::
DataArray (const DataArray_sequence& s)
{
  this->DataArray_ = s;
}


// Points
// 

const Points::DataArray_sequence& Points::
DataArray () const
{
  return this->DataArray_;
}

Points::DataArray_sequence& Points::
DataArray ()
{
  return this->DataArray_;
}

void Points::
DataArray (const DataArray_sequence& s)
{
  this->DataArray_ = s;
}


// Cells
// 

const Cells::DataArray_sequence& Cells::
DataArray () const
{
  return this->DataArray_;
}

Cells::DataArray_sequence& Cells::
DataArray ()
{
  return this->DataArray_;
}

void Cells::
DataArray (const DataArray_sequence& s)
{
  this->DataArray_ = s;
}


#include <xsd/cxx/xml/dom/parsing-source.hxx>

// DataArrayList_t
//

DataArrayList_t::
DataArrayList_t (const ::xercesc::DOMElement& e,
                 ::xml_schema::flags f,
                 ::xml_schema::container* c)
: ::xml_schema::simple_type (e, f, c),
  ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal > (e, f, this)
{
}

DataArrayList_t::
DataArrayList_t (const ::xercesc::DOMAttr& a,
                 ::xml_schema::flags f,
                 ::xml_schema::container* c)
: ::xml_schema::simple_type (a, f, c),
  ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal > (a, f, this)
{
}

DataArrayList_t::
DataArrayList_t (const ::std::string& s,
                 const ::xercesc::DOMElement* e,
                 ::xml_schema::flags f,
                 ::xml_schema::container* c)
: ::xml_schema::simple_type (s, e, f, c),
  ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal > (s, e, f, this)
{
}

DataArrayList_t* DataArrayList_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class DataArrayList_t (*this, f, c);
}

DataArrayList_t::
~DataArrayList_t ()
{
}

// DataArray_t
//

const DataArray_t::format_type DataArray_t::format_default_value_ (
  "ascii");

DataArray_t::
DataArray_t (const type_type& type,
             const Name_type& Name,
             const NumberOfComponents_type& NumberOfComponents)
: ::DataArrayList_t (),
  type_ (type, this),
  Name_ (Name, this),
  NumberOfComponents_ (NumberOfComponents, this),
  format_ (format_default_value (), this),
  offset_ (this)
{
}

DataArray_t::
DataArray_t (const ::DataArrayList_t& _xsd_DataArrayList_t_base,
             const type_type& type,
             const Name_type& Name,
             const NumberOfComponents_type& NumberOfComponents)
: ::DataArrayList_t (_xsd_DataArrayList_t_base),
  type_ (type, this),
  Name_ (Name, this),
  NumberOfComponents_ (NumberOfComponents, this),
  format_ (format_default_value (), this),
  offset_ (this)
{
}

DataArray_t::
DataArray_t (const DataArray_t& x,
             ::xml_schema::flags f,
             ::xml_schema::container* c)
: ::DataArrayList_t (x, f, c),
  type_ (x.type_, f, this),
  Name_ (x.Name_, f, this),
  NumberOfComponents_ (x.NumberOfComponents_, f, this),
  format_ (x.format_, f, this),
  offset_ (x.offset_, f, this)
{
}

DataArray_t::
DataArray_t (const ::xercesc::DOMElement& e,
             ::xml_schema::flags f,
             ::xml_schema::container* c)
: ::DataArrayList_t (e, f | ::xml_schema::flags::base, c),
  type_ (this),
  Name_ (this),
  NumberOfComponents_ (this),
  format_ (this),
  offset_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, false, false, true);
    this->parse (p, f);
  }
}

void DataArray_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  while (p.more_attributes ())
  {
    const ::xercesc::DOMAttr& i (p.next_attribute ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    if (n.name () == "type" && n.namespace_ ().empty ())
    {
      this->type_.set (type_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "Name" && n.namespace_ ().empty ())
    {
      this->Name_.set (Name_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "NumberOfComponents" && n.namespace_ ().empty ())
    {
      this->NumberOfComponents_.set (NumberOfComponents_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "format" && n.namespace_ ().empty ())
    {
      this->format_.set (format_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "offset" && n.namespace_ ().empty ())
    {
      this->offset_.set (offset_traits::create (i, f, this));
      continue;
    }
  }

  if (!type_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "type",
      "");
  }

  if (!Name_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "Name",
      "");
  }

  if (!NumberOfComponents_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "NumberOfComponents",
      "");
  }

  if (!format_.present ())
  {
    this->format_.set (format_default_value ());
  }
}

DataArray_t* DataArray_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class DataArray_t (*this, f, c);
}

DataArray_t& DataArray_t::
operator= (const DataArray_t& x)
{
  if (this != &x)
  {
    static_cast< ::DataArrayList_t& > (*this) = x;
    this->type_ = x.type_;
    this->Name_ = x.Name_;
    this->NumberOfComponents_ = x.NumberOfComponents_;
    this->format_ = x.format_;
    this->offset_ = x.offset_;
  }

  return *this;
}

DataArray_t::
~DataArray_t ()
{
}

// PieceUnstructuredGrid_t
//

PieceUnstructuredGrid_t::
PieceUnstructuredGrid_t (const PointData_type& PointData,
                         const CellData_type& CellData,
                         const Points_type& Points,
                         const Cells_type& Cells,
                         const NumberOfPoints_type& NumberOfPoints,
                         const NumberOfCells_type& NumberOfCells)
: ::xml_schema::type (),
  PointData_ (PointData, this),
  CellData_ (CellData, this),
  Points_ (Points, this),
  Cells_ (Cells, this),
  NumberOfPoints_ (NumberOfPoints, this),
  NumberOfCells_ (NumberOfCells, this)
{
}

PieceUnstructuredGrid_t::
PieceUnstructuredGrid_t (::std::auto_ptr< PointData_type > PointData,
                         ::std::auto_ptr< CellData_type > CellData,
                         ::std::auto_ptr< Points_type > Points,
                         ::std::auto_ptr< Cells_type > Cells,
                         const NumberOfPoints_type& NumberOfPoints,
                         const NumberOfCells_type& NumberOfCells)
: ::xml_schema::type (),
  PointData_ (PointData, this),
  CellData_ (CellData, this),
  Points_ (Points, this),
  Cells_ (Cells, this),
  NumberOfPoints_ (NumberOfPoints, this),
  NumberOfCells_ (NumberOfCells, this)
{
}

PieceUnstructuredGrid_t::
PieceUnstructuredGrid_t (const PieceUnstructuredGrid_t& x,
                         ::xml_schema::flags f,
                         ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  PointData_ (x.PointData_, f, this),
  CellData_ (x.CellData_, f, this),
  Points_ (x.Points_, f, this),
  Cells_ (x.Cells_, f, this),
  NumberOfPoints_ (x.NumberOfPoints_, f, this),
  NumberOfCells_ (x.NumberOfCells_, f, this)
{
}

PieceUnstructuredGrid_t::
PieceUnstructuredGrid_t (const ::xercesc::DOMElement& e,
                         ::xml_schema::flags f,
                         ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  PointData_ (this),
  CellData_ (this),
  Points_ (this),
  Cells_ (this),
  NumberOfPoints_ (this),
  NumberOfCells_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false, true);
    this->parse (p, f);
  }
}

void PieceUnstructuredGrid_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_content (); p.next_content (false))
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // PointData
    //
    if (n.name () == "PointData" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< PointData_type > r (
        PointData_traits::create (i, f, this));

      if (!PointData_.present ())
      {
        this->PointData_.set (r);
        continue;
      }
    }

    // CellData
    //
    if (n.name () == "CellData" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< CellData_type > r (
        CellData_traits::create (i, f, this));

      if (!CellData_.present ())
      {
        this->CellData_.set (r);
        continue;
      }
    }

    // Points
    //
    if (n.name () == "Points" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< Points_type > r (
        Points_traits::create (i, f, this));

      if (!Points_.present ())
      {
        this->Points_.set (r);
        continue;
      }
    }

    // Cells
    //
    if (n.name () == "Cells" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< Cells_type > r (
        Cells_traits::create (i, f, this));

      if (!Cells_.present ())
      {
        this->Cells_.set (r);
        continue;
      }
    }

    break;
  }

  if (!PointData_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "PointData",
      "");
  }

  if (!CellData_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "CellData",
      "");
  }

  if (!Points_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "Points",
      "");
  }

  if (!Cells_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "Cells",
      "");
  }

  while (p.more_attributes ())
  {
    const ::xercesc::DOMAttr& i (p.next_attribute ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    if (n.name () == "NumberOfPoints" && n.namespace_ ().empty ())
    {
      this->NumberOfPoints_.set (NumberOfPoints_traits::create (i, f, this));
      continue;
    }

    if (n.name () == "NumberOfCells" && n.namespace_ ().empty ())
    {
      this->NumberOfCells_.set (NumberOfCells_traits::create (i, f, this));
      continue;
    }
  }

  if (!NumberOfPoints_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "NumberOfPoints",
      "");
  }

  if (!NumberOfCells_.present ())
  {
    throw ::xsd::cxx::tree::expected_attribute< char > (
      "NumberOfCells",
      "");
  }
}

PieceUnstructuredGrid_t* PieceUnstructuredGrid_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class PieceUnstructuredGrid_t (*this, f, c);
}

PieceUnstructuredGrid_t& PieceUnstructuredGrid_t::
operator= (const PieceUnstructuredGrid_t& x)
{
  if (this != &x)
  {
    static_cast< ::xml_schema::type& > (*this) = x;
    this->PointData_ = x.PointData_;
    this->CellData_ = x.CellData_;
    this->Points_ = x.Points_;
    this->Cells_ = x.Cells_;
    this->NumberOfPoints_ = x.NumberOfPoints_;
    this->NumberOfCells_ = x.NumberOfCells_;
  }

  return *this;
}

PieceUnstructuredGrid_t::
~PieceUnstructuredGrid_t ()
{
}

// UnstructuredGrid_t
//

UnstructuredGrid_t::
UnstructuredGrid_t (const Piece_type& Piece)
: ::xml_schema::type (),
  Piece_ (Piece, this)
{
}

UnstructuredGrid_t::
UnstructuredGrid_t (::std::auto_ptr< Piece_type > Piece)
: ::xml_schema::type (),
  Piece_ (Piece, this)
{
}

UnstructuredGrid_t::
UnstructuredGrid_t (const UnstructuredGrid_t& x,
                    ::xml_schema::flags f,
                    ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  Piece_ (x.Piece_, f, this)
{
}

UnstructuredGrid_t::
UnstructuredGrid_t (const ::xercesc::DOMElement& e,
                    ::xml_schema::flags f,
                    ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  Piece_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false, false);
    this->parse (p, f);
  }
}

void UnstructuredGrid_t::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_content (); p.next_content (false))
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // Piece
    //
    if (n.name () == "Piece" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< Piece_type > r (
        Piece_traits::create (i, f, this));

      if (!Piece_.present ())
      {
        this->Piece_.set (r);
        continue;
      }
    }

    break;
  }

  if (!Piece_.present ())
  {
    throw ::xsd::cxx::tree::expected_element< char > (
      "Piece",
      "");
  }
}

UnstructuredGrid_t* UnstructuredGrid_t::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class UnstructuredGrid_t (*this, f, c);
}

UnstructuredGrid_t& UnstructuredGrid_t::
operator= (const UnstructuredGrid_t& x)
{
  if (this != &x)
  {
    static_cast< ::xml_schema::type& > (*this) = x;
    this->Piece_ = x.Piece_;
  }

  return *this;
}

UnstructuredGrid_t::
~UnstructuredGrid_t ()
{
}

// type
//

type::
type (const ::xercesc::DOMElement& e,
      ::xml_schema::flags f,
      ::xml_schema::container* c)
: ::xml_schema::string (e, f, c)
{
  _xsd_type_convert ();
}

type::
type (const ::xercesc::DOMAttr& a,
      ::xml_schema::flags f,
      ::xml_schema::container* c)
: ::xml_schema::string (a, f, c)
{
  _xsd_type_convert ();
}

type::
type (const ::std::string& s,
      const ::xercesc::DOMElement* e,
      ::xml_schema::flags f,
      ::xml_schema::container* c)
: ::xml_schema::string (s, e, f, c)
{
  _xsd_type_convert ();
}

type* type::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class type (*this, f, c);
}

type::value type::
_xsd_type_convert () const
{
  ::xsd::cxx::tree::enum_comparator< char > c (_xsd_type_literals_);
  const value* i (::std::lower_bound (
                    _xsd_type_indexes_,
                    _xsd_type_indexes_ + 10,
                    *this,
                    c));

  if (i == _xsd_type_indexes_ + 10 || _xsd_type_literals_[*i] != *this)
  {
    throw ::xsd::cxx::tree::unexpected_enumerator < char > (*this);
  }

  return *i;
}

const char* const type::
_xsd_type_literals_[10] =
{
  "Int8",
  "UInt8",
  "Int16",
  "UInt16",
  "Int32",
  "UInt32",
  "Int64",
  "UInt64",
  "Float32",
  "Float64"
};

const type::value type::
_xsd_type_indexes_[10] =
{
  ::type::Float32,
  ::type::Float64,
  ::type::Int16,
  ::type::Int32,
  ::type::Int64,
  ::type::Int8,
  ::type::UInt16,
  ::type::UInt32,
  ::type::UInt64,
  ::type::UInt8
};

// PointData
//

PointData::
PointData ()
: ::xml_schema::type (),
  DataArray_ (this)
{
}

PointData::
PointData (const PointData& x,
           ::xml_schema::flags f,
           ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  DataArray_ (x.DataArray_, f, this)
{
}

PointData::
PointData (const ::xercesc::DOMElement& e,
           ::xml_schema::flags f,
           ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  DataArray_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false, false);
    this->parse (p, f);
  }
}

void PointData::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_content (); p.next_content (false))
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // DataArray
    //
    if (n.name () == "DataArray" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< DataArray_type > r (
        DataArray_traits::create (i, f, this));

      this->DataArray_.push_back (r);
      continue;
    }

    break;
  }
}

PointData* PointData::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class PointData (*this, f, c);
}

PointData& PointData::
operator= (const PointData& x)
{
  if (this != &x)
  {
    static_cast< ::xml_schema::type& > (*this) = x;
    this->DataArray_ = x.DataArray_;
  }

  return *this;
}

PointData::
~PointData ()
{
}

// CellData
//

CellData::
CellData ()
: ::xml_schema::type (),
  DataArray_ (this)
{
}

CellData::
CellData (const CellData& x,
          ::xml_schema::flags f,
          ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  DataArray_ (x.DataArray_, f, this)
{
}

CellData::
CellData (const ::xercesc::DOMElement& e,
          ::xml_schema::flags f,
          ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  DataArray_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false, false);
    this->parse (p, f);
  }
}

void CellData::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_content (); p.next_content (false))
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // DataArray
    //
    if (n.name () == "DataArray" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< DataArray_type > r (
        DataArray_traits::create (i, f, this));

      this->DataArray_.push_back (r);
      continue;
    }

    break;
  }
}

CellData* CellData::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class CellData (*this, f, c);
}

CellData& CellData::
operator= (const CellData& x)
{
  if (this != &x)
  {
    static_cast< ::xml_schema::type& > (*this) = x;
    this->DataArray_ = x.DataArray_;
  }

  return *this;
}

CellData::
~CellData ()
{
}

// Points
//

Points::
Points ()
: ::xml_schema::type (),
  DataArray_ (this)
{
}

Points::
Points (const Points& x,
        ::xml_schema::flags f,
        ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  DataArray_ (x.DataArray_, f, this)
{
}

Points::
Points (const ::xercesc::DOMElement& e,
        ::xml_schema::flags f,
        ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  DataArray_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false, false);
    this->parse (p, f);
  }
}

void Points::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_content (); p.next_content (false))
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // DataArray
    //
    if (n.name () == "DataArray" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< DataArray_type > r (
        DataArray_traits::create (i, f, this));

      this->DataArray_.push_back (r);
      continue;
    }

    break;
  }
}

Points* Points::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class Points (*this, f, c);
}

Points& Points::
operator= (const Points& x)
{
  if (this != &x)
  {
    static_cast< ::xml_schema::type& > (*this) = x;
    this->DataArray_ = x.DataArray_;
  }

  return *this;
}

Points::
~Points ()
{
}

// Cells
//

Cells::
Cells ()
: ::xml_schema::type (),
  DataArray_ (this)
{
}

Cells::
Cells (const Cells& x,
       ::xml_schema::flags f,
       ::xml_schema::container* c)
: ::xml_schema::type (x, f, c),
  DataArray_ (x.DataArray_, f, this)
{
}

Cells::
Cells (const ::xercesc::DOMElement& e,
       ::xml_schema::flags f,
       ::xml_schema::container* c)
: ::xml_schema::type (e, f | ::xml_schema::flags::base, c),
  DataArray_ (this)
{
  if ((f & ::xml_schema::flags::base) == 0)
  {
    ::xsd::cxx::xml::dom::parser< char > p (e, true, false, false);
    this->parse (p, f);
  }
}

void Cells::
parse (::xsd::cxx::xml::dom::parser< char >& p,
       ::xml_schema::flags f)
{
  for (; p.more_content (); p.next_content (false))
  {
    const ::xercesc::DOMElement& i (p.cur_element ());
    const ::xsd::cxx::xml::qualified_name< char > n (
      ::xsd::cxx::xml::dom::name< char > (i));

    // DataArray
    //
    if (n.name () == "DataArray" && n.namespace_ ().empty ())
    {
      ::std::auto_ptr< DataArray_type > r (
        DataArray_traits::create (i, f, this));

      this->DataArray_.push_back (r);
      continue;
    }

    break;
  }
}

Cells* Cells::
_clone (::xml_schema::flags f,
        ::xml_schema::container* c) const
{
  return new class Cells (*this, f, c);
}

Cells& Cells::
operator= (const Cells& x)
{
  if (this != &x)
  {
    static_cast< ::xml_schema::type& > (*this) = x;
    this->DataArray_ = x.DataArray_;
  }

  return *this;
}

Cells::
~Cells ()
{
}

#include <istream>
#include <xsd/cxx/xml/sax/std-input-source.hxx>
#include <xsd/cxx/tree/error-handler.hxx>

#include <ostream>
#include <xsd/cxx/tree/error-handler.hxx>
#include <xsd/cxx/xml/dom/serialization-source.hxx>

void
operator<< (::xercesc::DOMElement& e, const DataArrayList_t& i)
{
  e << static_cast< const ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal >& > (i);
}

void
operator<< (::xercesc::DOMAttr& a, const DataArrayList_t& i)
{
  a << static_cast< const ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal >& > (i);
}

void
operator<< (::xml_schema::list_stream& l,
            const DataArrayList_t& i)
{
  l << static_cast< const ::xsd::cxx::tree::list< ::xml_schema::decimal, char, ::xsd::cxx::tree::schema_type::decimal >& > (i);
}

void
operator<< (::xercesc::DOMElement& e, const DataArray_t& i)
{
  e << static_cast< const ::DataArrayList_t& > (i);

  // type
  //
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "type",
        e));

    a << i.type ();
  }

  // Name
  //
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "Name",
        e));

    a << i.Name ();
  }

  // NumberOfComponents
  //
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "NumberOfComponents",
        e));

    a << i.NumberOfComponents ();
  }

  // format
  //
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "format",
        e));

    a << i.format ();
  }

  // offset
  //
  if (i.offset ())
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "offset",
        e));

    a << *i.offset ();
  }
}

void
operator<< (::xercesc::DOMElement& e, const PieceUnstructuredGrid_t& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // PointData
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "PointData",
        e));

    s << i.PointData ();
  }

  // CellData
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "CellData",
        e));

    s << i.CellData ();
  }

  // Points
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "Points",
        e));

    s << i.Points ();
  }

  // Cells
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "Cells",
        e));

    s << i.Cells ();
  }

  // NumberOfPoints
  //
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "NumberOfPoints",
        e));

    a << i.NumberOfPoints ();
  }

  // NumberOfCells
  //
  {
    ::xercesc::DOMAttr& a (
      ::xsd::cxx::xml::dom::create_attribute (
        "NumberOfCells",
        e));

    a << i.NumberOfCells ();
  }
}

void
operator<< (::xercesc::DOMElement& e, const UnstructuredGrid_t& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // Piece
  //
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "Piece",
        e));

    s << i.Piece ();
  }
}

void
operator<< (::xercesc::DOMElement& e, const type& i)
{
  e << static_cast< const ::xml_schema::string& > (i);
}

void
operator<< (::xercesc::DOMAttr& a, const type& i)
{
  a << static_cast< const ::xml_schema::string& > (i);
}

void
operator<< (::xml_schema::list_stream& l,
            const type& i)
{
  l << static_cast< const ::xml_schema::string& > (i);
}

void
operator<< (::xercesc::DOMElement& e, const PointData& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // DataArray
  //
  for (PointData::DataArray_const_iterator
       b (i.DataArray ().begin ()), n (i.DataArray ().end ());
       b != n; ++b)
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "DataArray",
        e));

    s << *b;
  }
}

void
operator<< (::xercesc::DOMElement& e, const CellData& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // DataArray
  //
  for (CellData::DataArray_const_iterator
       b (i.DataArray ().begin ()), n (i.DataArray ().end ());
       b != n; ++b)
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "DataArray",
        e));

    s << *b;
  }
}

void
operator<< (::xercesc::DOMElement& e, const Points& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // DataArray
  //
  for (Points::DataArray_const_iterator
       b (i.DataArray ().begin ()), n (i.DataArray ().end ());
       b != n; ++b)
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "DataArray",
        e));

    s << *b;
  }
}

void
operator<< (::xercesc::DOMElement& e, const Cells& i)
{
  e << static_cast< const ::xml_schema::type& > (i);

  // DataArray
  //
  for (Cells::DataArray_const_iterator
       b (i.DataArray ().begin ()), n (i.DataArray ().end ());
       b != n; ++b)
  {
    ::xercesc::DOMElement& s (
      ::xsd::cxx::xml::dom::create_element (
        "DataArray",
        e));

    s << *b;
  }
}

#include <xsd/cxx/post.hxx>

// Begin epilogue.
//
//
// End epilogue.

