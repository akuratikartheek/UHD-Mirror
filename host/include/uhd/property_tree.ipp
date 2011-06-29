//
// Copyright 2011 Ettus Research LLC
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef INCLUDED_UHD_PROPERTY_TREE_IPP
#define INCLUDED_UHD_PROPERTY_TREE_IPP

#include <boost/foreach.hpp>
#include <vector>

/***********************************************************************
 * Implement templated property impl
 **********************************************************************/
namespace uhd{ namespace /*anon*/{

template <typename T> class UHD_API property_impl : public property<T>{
public:

    property<T> &subscribe_master(const typename property<T>::master_type &master){
        _master = master;
        return *this;
    }

    property<T> &publish(const typename property<T>::publisher_type &publisher){
        _publisher = publisher;
        return *this;
    }

    property<T> &subscribe(const typename property<T>::subscriber_type &subscriber){
        _subscribers.push_back(subscriber);
        return *this;
    }

    property<T> &update(void){
        this->set(this->get());
        return *this;
    }

    property<T> &set(const T &value){
        _value = boost::shared_ptr<T>(new T(_master.empty()? value : _master(value)));
        BOOST_FOREACH(typename property<T>::subscriber_type &subscriber, _subscribers){
            subscriber(*_value); //let errors propagate
        }
        return *this;
    }

    T get(void) const{
        return _publisher.empty()? *_value : _publisher();
    }

private:
    std::vector<typename property<T>::subscriber_type> _subscribers;
    typename property<T>::publisher_type _publisher;
    typename property<T>::master_type _master;
    boost::shared_ptr<T> _value;
};

}} //namespace uhd::/*anon*/

/***********************************************************************
 * Implement templated methods for the property tree
 **********************************************************************/
namespace uhd{

    template <typename T> property<T> &property_tree::create(const path_type &path){
        this->_create(path, typename boost::shared_ptr<property<T> >(new property_impl<T>()));
        return this->access<T>(path);
    }

    template <typename T> property<T> &property_tree::access(const path_type &path){
        return *boost::static_pointer_cast<property<T> >(this->_access(path));
    }

} //namespace uhd

#endif /* INCLUDED_UHD_PROPERTY_TREE_IPP */
