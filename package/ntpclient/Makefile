# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id: Makefile,v 1.1.4.3 2007/02/27 03:06:36 ronger Exp $

include $(TOPDIR)/rules.mk

PKG_NAME:=ntpclient
PKG_RELEASE:=4
PKG_BUILD_DEPENDS:=datalib
PKG_BUILD_DIR := $(BUILD_DIR)/$(PKG_NAME)

PKG_GIT_TREEISH=ntpclient_2003_194-dni33

include $(INCLUDE_DIR)/package.mk

define Package/ntpclient
  SECTION:=utils
  CATEGORY:=Base system
  DEPENDS:=@TARGET_wnr2000v4
  TITLE:=The CGI interface
endef

define Package/ntpclient/description
	The CGI interface
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/$(PKG_NAME).git/* $(BUILD_DIR)/$(PKG_NAME)/
	$(CP) ./files/dniconfig.h $(PKG_BUILD_DIR)/
endef

define Package/ntpclient/install
	install -d -m0755 $(1)/usr/sbin
	install -m0755 $(PKG_BUILD_DIR)/ntpclient $(1)/usr/sbin/
	install -d -m0755 $(1)/etc/init.d
	install -m0755 ./files/ntpclient.init $(1)/etc/init.d/ntpclient
endef

$(eval $(call BuildPackage,ntpclient))
