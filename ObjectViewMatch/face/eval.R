# Load libraries
library(ggplot2)
library(gplots)
library(reshape)
library(scales)

# Read CSVs
data <- NULL
tmp <- read.csv("eval_1A.csv")
tmp$File <- "eval_1A"
data <- rbind(data, tmp)

# Split data into individual plots
plot_index = which(names(data)=="Plot")
Metadata <- data[grep("Metadata",data$Plot),-c(1)]
TI <- data[grep("TI",data$Plot),-c(1)]
BG <- data[grep("BG",data$Plot),-c(1)]
DET <- data[grep("DET",data$Plot),-c(1)]
FAR <- data[grep("FAR",data$Plot),-c(1)]
FRR <- data[grep("FRR",data$Plot),-c(1)]
SD <- data[grep("SD",data$Plot),-c(1)]
FT <- data[grep("FT",data$Plot),-c(1)]
CT <- data[grep("CT",data$Plot),-c(1)]
BC <- data[grep("BC",data$Plot),-c(1)]
CMC <- data[grep("CMC",data$Plot),-c(1)]
FAR$Error <- "FAR"
FRR$Error <- "FRR"
ERR <- rbind(FAR, FRR)
rm(data, FAR, FRR)

# Format data
Metadata$Y<-factor(Metadata$Y, levels=c("Genuine","Impostor","Ignored","Gallery","Probe"))
TI$Y <- as.character(TI$Y)
BG$Y <- as.character(BG$Y)
DET$Y <- as.numeric(as.character(DET$Y))
ERR$Y <- as.numeric(as.character(ERR$Y))
SD$Y <- as.factor(unique(as.character(SD$Y)))
FT$Y <- as.numeric(as.character(FT$Y))
CT$Y <- as.numeric(as.character(CT$Y))
BC$Y <- as.numeric(as.character(BC$Y))
CMC$Y <- as.numeric(as.character(CMC$Y))

# Code to format FAR values
far_names <- list('0.001'="FAR = 0.1%", '0.01'="FAR = 1%")
far_labeller <- function(variable,value) { return(far_names[as.character(value)]) }

# Code to format TAR@FAR table
algs <- unique(FT$File)
algs <- algs[!duplicated(algs)]
mat <- matrix(FT$Y,nrow=6,ncol=length(algs),byrow=FALSE)
colnames(mat) <- algs 
rownames(mat) <- c("FAR = 1e-06", "FAR = 1e-05", "FAR = 1e-04", "FAR = 1e-03", "FAR = 1e-02", "FAR = 1e-01")
FTtable <- as.table(mat)

# Code to format CMC Table
mat <- matrix(CT$Y,nrow=6,ncol=length(algs),byrow=FALSE)
colnames(mat) <- algs 
rownames(mat) <- c(" Rank 1", "Rank 5", "Rank 10", "Rank 20", "Rank 50", "Rank 100")
CMCtable <- as.table(mat)

# Open output device
pdf("./eval.pdf")

# Write metadata table
MT <- as.data.frame(Metadata[c(1,2,3,4,5),])
par(mfrow=c(4,1))
plot.new()
print(title(paste("OpenBR - 0.5.0",date(),sep="\n")))
mat <- matrix(MT$X[c(1,2)],ncol=2)
colnames(mat) <- c("Gallery", "Probe")
imageTable <- as.table(mat)
print(textplot(imageTable,show.rownames=FALSE))
print(title("Images"))
mat <- matrix(MT$X[c(3,4,5)],ncol=3)
colnames(mat) <- c("Genuine", "Impostor", "Ignored")
matchTable <- as.table(mat)
print(textplot(matchTable,show.rownames=FALSE))
print(title("Matches"))
plot.new()
print(title("Gallery * Probe = Genuine + Impostor + Ignored"))
plot.new()
print(textplot(FTtable))
print(title("Table of True Accept Rates at various False Accept Rates"))
print(textplot(CMCtable))
print(title("Table of retrieval rate at various ranks"))

# Write figures
qplot(X, 1-Y, data=DET, geom="line", main="", xlab="False Accept Rate", ylab="True Accept Rate") + theme_minimal() + scale_x_log10(labels=trans_format("log10", math_format())) + scale_y_continuous(labels=percent) + annotation_logticks(sides="b") + theme(legend.title = element_text(size = 12), plot.title = element_text(size = 12), axis.text = element_text(size = 12), axis.title.x = element_text(size = 12), axis.title.y = element_text(size = 12), legend.position='bottom', legend.background = element_rect(fill = 'white'), panel.grid.major = element_line(colour = "gray"), panel.grid.minor = element_line(colour = "gray", linetype = "dashed"), legend.text = element_text(size = 12))

qplot(X, Y, data=DET, geom="line", xlab="False Accept Rate", ylab="False Reject Rate") + geom_abline(alpha=0.5, colour="grey", linetype="dashed") + theme_minimal() + scale_x_log10(labels=trans_format("log10", math_format())) + scale_y_log10(labels=trans_format("log10", math_format())) + annotation_logticks()

qplot(X, data=SD, geom="histogram", fill=Y, position="identity", alpha=I(1/2), xlab="Score", ylab="Frequency") + scale_fill_manual("Ground Truth", values=c("blue", "red")) + theme_minimal() + scale_x_continuous(minor_breaks=NULL) + scale_y_continuous(minor_breaks=NULL) + theme(axis.text.y=element_blank(), axis.ticks=element_blank(), axis.text.x=element_text(angle=-90, hjust=0)) + theme(aspect.ratio=1)

ggplot(CMC, aes(x=X, y=Y)) + ggtitle("") + xlab("Rank") + ylab("Retrieval Rate") + geom_line(size=1) + scale_x_log10(labels=c(1,5,10,50,100), breaks=c(1,5,10,50,100)) + annotation_logticks(sides="b") + scale_y_continuous(labels=percent) + theme_minimal() + theme(legend.title = element_text(size = 12), plot.title = element_text(size = 12), axis.text = element_text(size = 12), axis.title.x = element_text(size = 12), axis.title.y = element_text(size = 12), legend.position='bottom', legend.background = element_rect(fill = 'white'), panel.grid.major = element_line(colour = "gray"), panel.grid.minor = element_line(colour = "gray", linetype = "dashed"), legend.text = element_text(size = 12))

qplot(factor(File), data=BC, geom="bar", position="dodge", weight=Y, xlab="False Accept Rate", ylab="True Accept Rate") + theme_minimal() + facet_grid(. ~ X, labeller=far_labeller) + scale_y_continuous(labels=percent) + theme(legend.position="none", axis.text.x=element_text(angle=-90, hjust=0)) + geom_text(data=BC, aes(label=Y, y=0.05))

qplot(X, Y, data=ERR, geom="line", linetype=Error, xlab="Score", ylab="Error Rate") + theme_minimal() + scale_y_log10(labels=percent) + annotation_logticks(sides="l") + theme(aspect.ratio=1)

if (nrow(TI) != 0) {
	library(jpeg)
	library(png)
	library(tiff)
	library(grid)
	multiplot <- function(..., plotlist=NULL, cols) {
		require(grid)

		# Make a list from the ... arguments and plotlist
		plots <- c(list(...), plotlist)
		numPlots = length(plots)

		# Make the panel
		plotCols = cols
		plotRows = ceiling(numPlots/plotCols)

		# Set up the page
		grid.newpage()
		pushViewport(viewport(layout = grid.layout(plotRows, plotCols)))
		vplayout <- function(x, y)
			viewport(layout.pos.row = x, layout.pos.col = y)

		# Make each plot, in the correct location
		for (i in 1:numPlots) {
			curRow = ceiling(i/plotCols)
			curCol = (i-1) %% plotCols + 1
			print(plots[[i]], vp = vplayout(curRow, curCol))
		}
	}

	# Print top impostor matches
	for (i in 1:nrow(TI)) {
		score <- TI[i,1]
		files <- TI[i,2]
		alg <- TI[i,3]
		files <- unlist(strsplit(files, "[:]"))

		ext1 <- unlist(strsplit(files[2], "[.]"))[2]
		ext2 <- unlist(strsplit(files[4], "[.]"))[2]
		if (ext1 == "jpg" || ext1 == "JPEG" || ext1 == "jpeg" || ext1 == "JPG") {
			img1 <- readJPEG(files[2])
		} else if (ext1 == "PNG" || ext1 == "png") {
			img1 <- readPNG(files[2])
		} else if (ext1 == "TIFF" || ext1 == "tiff" || ext1 == "TIF" || ext1 == "tif") {
			img1 <- readTIFF(files[2])
		} else {
			next
		}
		if (ext2 == "jpg" || ext2 == "JPEG" || ext2 == "jpeg" || ext2 == "JPG") {
			img2 <- readJPEG(files[4])
		} else if (ext2 == "PNG" || ext2 == "png") {
			img2 <- readPNG(files[4])
		} else if (ext2 == "TIFF" || ext2 == "tiff" || ext2 == "TIF" || ext2 == "tif") {
			img2 <- readTIFF(files[4])
		} else {
			next
		}
		name1 <- files[1]
		name2 <- files[3]

		g1 <- rasterGrob(img1, interpolate=TRUE)
		g2 <- rasterGrob(img2, interpolate=TRUE)

		plot1 <- qplot(1:10, 1:10, geom="blank") + annotation_custom(g1, xmin=-Inf, xmax=Inf, ymin=-Inf, ymax=Inf) + theme(axis.line=element_blank(), axis.text.x=element_blank(), axis.text.y=element_blank(), axis.ticks=element_blank(), panel.background=element_blank()) + labs(title=alg) + ylab(unlist(strsplit(files[2], "[/]"))[length(unlist(strsplit(files[2], "[/]")))]) + xlab(name1)
		plot2 <- qplot(1:10, 1:10, geom="blank") + annotation_custom(g2, xmin=-Inf, xmax=Inf, ymin=-Inf, ymax=Inf) + theme(axis.line=element_blank(), axis.text.x=element_blank(), axis.text.y=element_blank(), axis.ticks=element_blank(), panel.background=element_blank()) + labs(title=paste("Impostor score =", score)) + ylab(unlist(strsplit(files[4], "[/]"))[length(unlist(strsplit(files[4], "[/]")))]) + xlab(name2)

		multiplot(plot1, plot2, cols=2)
	}

	# Print worst genuine matches
	for (i in 1:nrow(BG)) {
		score <- BG[i,1]
		files <- BG[i,2]
		alg <- BG[i,3]
		files <- unlist(strsplit(files, "[:]"))

		ext1 <- unlist(strsplit(files[2], "[.]"))[2]
		ext2 <- unlist(strsplit(files[4], "[.]"))[2]
		if (ext1 == "jpg" || ext1 == "JPEG" || ext1 == "jpeg" || ext1 == "JPG") {
			img1 <- readJPEG(files[2])
		} else if (ext1 == "PNG" || ext1 == "png") {
			img1 <- readPNG(files[2])
		} else if (ext1 == "TIFF" || ext1 == "tiff" || ext1 == "TIF" || ext1 == "tif") {
			img1 <- readTIFF(files[2])
		} else {
			next
		}
		if (ext2 == "jpg" || ext2 == "JPEG" || ext2 == "jpeg" || ext2 == "JPG") {
			img2 <- readJPEG(files[4])
		} else if (ext2 == "PNG" || ext2 == "png") {
			img2 <- readPNG(files[4])
		} else if (ext2 == "TIFF" || ext2 == "tiff" || ext2 == "TIF" || ext2 == "tif") {
			img2 <- readTIFF(files[4])
		} else {
			next
		}
		name1 <- files[1]
		name2 <- files[3]

		g1 <- rasterGrob(img1, interpolate=TRUE)
		g2 <- rasterGrob(img2, interpolate=TRUE)

		plot1 <- qplot(1:10, 1:10, geom="blank") + annotation_custom(g1, xmin=-Inf, xmax=Inf, ymin=-Inf, ymax=Inf) + theme(axis.line=element_blank(), axis.text.x=element_blank(), axis.text.y=element_blank(), axis.ticks=element_blank(), panel.background=element_blank()) + labs(title=alg) + ylab(unlist(strsplit(files[2], "[/]"))[length(unlist(strsplit(files[2], "[/]")))]) + xlab(name1)
		plot2 <- qplot(1:10, 1:10, geom="blank") + annotation_custom(g2, xmin=-Inf, xmax=Inf, ymin=-Inf, ymax=Inf) + theme(axis.line=element_blank(), axis.text.x=element_blank(), axis.text.y=element_blank(), axis.ticks=element_blank(), panel.background=element_blank()) + labs(title=paste("Genuine score =", score)) + ylab(unlist(strsplit(files[4], "[/]"))[length(unlist(strsplit(files[4], "[/]")))]) + xlab(name2)

		multiplot(plot1, plot2, cols=2)
	}
}

dev.off()
