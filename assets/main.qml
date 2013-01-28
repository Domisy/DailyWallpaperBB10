// Default empty project template
import bb.cascades 1.0
import bb.system 1.0
import org.labsquare 1.0

Page {
    titleBar: TitleBar {
        title: "The Bing Image of the Day"
    }
    Menu.definition: MenuDefinition {
        // Add a Help action
        helpAction: HelpActionItem {
            onTriggered: {
                aboutDialog.show()
            }
        }
        attachedObjects: [
            SystemDialog {
                id: aboutDialog
                title: "About"
                body: "Daily Wallpaper was brought to you by - Domisy Dev\n\nLead Developer - Theodore Mavrakis\n\nSupport - domisydev@gmail.com\nVersion - 1.0"
            }
        ]
    }
    actions: [
        ActionItem {
            title: "Refresh"
            imageSource: "asset:///images/reloadIcon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                //app.initiateRequest(bingWebImage);
                app.initiateRequest();
            }
        },
        ActionItem {
            title: "Set Wallpaper"
            imageSource: "asset:///images/setBackgroundIcon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                app.setImageWallpaper(bingWebImage.url);
            }
        },
        ActionItem {
            title: "Info"
            imageSource: "asset:///images/aboutIcon.png"
            ActionBar.placement: ActionBarPlacement.OnBar
            onTriggered: {
                app.showImageInfoToast();
            }
        }
    ]
    Container {
        layout: DockLayout {
        }
        ActivityIndicator {
            objectName: "indicator"
            id: indicator
            verticalAlignment: VerticalAlignment.Center
            horizontalAlignment: HorizontalAlignment.Center
            preferredWidth: 200
            preferredHeight: 200
        }
        WebImageView {
            objectName: "bingImageComponent"
            id: bingWebImage
            preferredHeight: maxHeight - 200
            preferredWidth: maxWidth
            scalingMethod: ScalingMethod.AspectFill
            horizontalAlignment: HorizontalAlignment.Center
            verticalAlignment: VerticalAlignment.Center
        }
    }
}
