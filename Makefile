include $(TOPDIR)/rules.mk

PKG_NAME:=mm
PKG_VERSION:=0.1
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/uloop/Default
  SECTION:=utils
  CATEGORY:=Utilities
endef

define Package/uloop/description
 This package contains the uloop necessary files
endef

define Package/mm
  SUBMENU=uloop
  TITLE:=ULOOP MM
  DEPENDS:=+uloop-messages
endef

define Package/mm/description
	ULOOP Openwrt APP for Reward Manager
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) ./src/* $(PKG_BUILD_DIR)
endef

define Package/mm/install
	$(INSTALL_DIR) $(1)/bin
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/MM $(1)/bin/
endef

$(eval $(call BuildPackage,mm))
