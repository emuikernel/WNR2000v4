# 
# Copyright (C) 2006 OpenWrt.org
#
# This is free software, licensed under the GNU General Public License v2.
# See /LICENSE for more information.
#
# $Id$

include $(TOPDIR)/rules.mk
include $(INCLUDE_DIR)/kernel.mk

PKG_NAME:=kmod-ipv6-cone
PKG_RELEASE:=1

PKG_BUILD_DIR:=$(KERNEL_BUILD_DIR)/$(PKG_NAME)
PKG_GIT_TREEISH:=kmod-ipv6-cone-v1.0.1-dni8

include $(INCLUDE_DIR)/package.mk

ifneq ($(CONFIG_TARGET_wnr2000v4),)
  MODULES:=
else
  MODULES:=ip6t_CONE
endif

define KernelPackage/ipv6-cone
  SUBMENU:=Other modules
  DEPENDS:=@TARGET_wnr2000v4
  TITLE:=IPv6 CONE linux kernel module for 2.6.31
  VERSION:=$(LINUX_VERSION)-$(PKG_RELEASE)
  FILES:= \
	$(PKG_BUILD_DIR)/ip6t_CONE.$(LINUX_KMOD_SUFFIX)
  AUTOLOAD:=$(call AutoLoad,46,$(MODULES))
endef

define Build/Prepare
	$(CP) $(GIT_HOME)/$(PKG_NAME).git/* $(KERNEL_BUILD_DIR)/$(PKG_NAME)
endef

#ifeq ($(BOARD),brcm)
#BUILDFLAGS := -DBROADCOM -DBCMDRIVER -I$(LINUX_DIR)/arch/mips/bcm947xx/include
#else
#ifeq ($(BOARD),brcm47xx)
#BUILDFLAGS := -DBROADCOM
#endif
#endif


define Build/Compile
endef

define KernelPackage/ipv6-cone/install
	$(INSTALL_DIR) $(1)/lib/network/
endef

$(eval $(call KernelPackage,ipv6-cone))
